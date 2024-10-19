#include "server/server.h"

Server::Server(int numClients) : running(true){
    init();
}

// Run the server by calling the server loop.
void Server::run(){
    // Create a thread for the package receive loop.
    threads.push_back(std::thread(&Server::packageRecLoop, this));
    // Create a thread for the package send loop.
    threads.push_back(std::thread(&Server::packageSendLoop, this));

    loop();
}

void Server::init() {
    // Create server socket
    serverSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (serverSocket < 0) {
        throw std::runtime_error("Failed to create socket");
    }

    // Specify the address
    sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));  // Zero out the structure
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(49154);

    // INADDR_ANY makes the server to listen on all interfaces
    server_address.sin_addr.s_addr = INADDR_ANY;

    // Bind the socket with the server address
    if (bind(serverSocket, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
        close(serverSocket);
        throw std::runtime_error("Failed to bind socket");
    }

    std::cout << "SERVER: " << inet_ntoa(server_address.sin_addr) 
              << " Port: " << ntohs(server_address.sin_port) << std::endl;
}

// Start the server loop.
void Server::loop(){
    std::cout << "Server is running..." << std::endl;
    while(running.load()){
        // Handle server closing logic (storing current data etc.)
    }
}

// Read packages from clients
// Used by Receive Thread
void Server::packageRecLoop(){
    std::cout << "Server is recieving packages..." << std::endl;
    while (running.load()) {
        // Recieve packages from clients
        recievePackage();
    }
}

// Write packages to clients
// Used by Send Thread
void Server::packageSendLoop(){
    std::cout << "Server is sending packages..." << std::endl;
    while (running.load()) {
        // Send packages to clients
        sendPackage();
    }
}

void Server::recievePackage() {
    Package package;
    sockaddr_in clientAddress;
    socklen_t clientAddressLen = sizeof(clientAddress);
    ssize_t recvBytes = recvfrom(serverSocket, &package, sizeof(Package), 0, (struct sockaddr*)&clientAddress, &clientAddressLen);
    if (recvBytes == -1) {
        perror("recvfrom failed");
        return;
    }
    if (package.type == PLAYER_CONNECT) {
        std::cout << "Player connected" << std::endl;
        // Generate a new client ID
        boost::uuids::uuid clientID = boost::uuids::random_generator()();
        // Only store the client address once when it is created by the server.
        mtx.lock();
        clients[clientID] = clientInfo(false,  clientAddress);
        // Store the package in the map with the updated client ID.
        Package initPackage(clientID);
        packages[clientID] = initPackage;
        mtx.unlock();

    } else if (package.type == PLAYER_INFO) {
        // Add the package to the map
        mtx.lock();
        clients[package.clientID] = clientInfo(true,  clientAddress);
        packages[package.clientID] = package;
        mtx.unlock();
    }
    else if (package.type == PLAYER_DISCONNECT) {
        std::cout << "Player disconnected" << std::endl;
        // Remove the client from the map
        mtx.lock();
        clients.erase(package.clientID);
        packages[package.clientID] = package;
        mtx.unlock();
    }
    else {
        std::cout << "Unknown package type" << std::endl;
    }
}

void Server::sendPackage() {
    if (clients.size() == 0 || packages.size() == 0) {
        return;
    }
    // Have local maps so that data races are avoided.
    mtx.lock();
    std::unordered_map<boost::uuids::uuid, Package> localPackages = packages;
    std::unordered_map<boost::uuids::uuid, clientInfo> localClients = clients;
    mtx.unlock();

    // Go through each package and send it to all clients.
    for (auto it = localPackages.begin(); it != localPackages.end(); ){
        if (it->second.clientID == boost::uuids::nil_uuid()) {
            it++;
            continue;
        }
        if (it->second.type == PLAYER_CONNECT && localClients[it->second.clientID].connected == false) {
            std::cout << "Sending PLAYER_CONNECT package to client: " << it->second.clientID << std::endl;
            ssize_t sentBytes = sendto(serverSocket, &it->second, sizeof(Package), 0, (struct sockaddr*)&localClients[it->second.clientID].address, sizeof(localClients[it->second.clientID].address));
            if (sentBytes == -1) {
                perror("sendto failed");
                return;
            }
            std::cout << "Sent PLAYER_CONNECT package to client: " << it->second.clientID << std::endl;
            mtx.lock();
            clients[it->second.clientID].connected = true;
            mtx.unlock();
            it++;
        }
        else if (it->second.type == PLAYER_DISCONNECT) {
            for (auto const& y : localClients) {
                if (!y.second.connected) continue;
                if (it->first == y.first) continue;
                std::cout << "Sending PLAYER_DISCONNECT package to client: " << y.first << std::endl;
                ssize_t sentBytes = sendto(serverSocket, &it->second, sizeof(Package), 0, (struct sockaddr*)&y.second.address, sizeof(y.second.address));
                if (sentBytes == -1) {
                    perror("sendto failed");
                    return;
                }
            }
            mtx.lock();
            packages.erase(it->first);
            mtx.unlock();
            it = localPackages.erase(it);
        }
        else {
            for (auto const& y : localClients) {
                if (!y.second.connected) continue;
                if (it->first == y.first) continue;

                ssize_t sentBytes = sendto(serverSocket, &it->second, sizeof(Package), 0, (struct sockaddr*)&y.second.address, sizeof(y.second.address));
                if (sentBytes == -1) {
                    perror("sendto failed");
                    return;
                }
            }
            it++;
        }
        // Sleep for 5ms
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
}

// Cleanup the server.
void Server::cleanup(){
    close(serverSocket);
}

Server::~Server(){
    cleanup();
}
