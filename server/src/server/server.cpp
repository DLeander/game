#include "server/server.h"

CSERVER::CSERVER(int iClients) : m_abRunning(true){
    init();
}

// Run the server by calling the server loop.
void CSERVER::run(){
    // Create a thread for the package receive loop.
    m_vThreads.push_back(std::thread(&CSERVER::packageRecLoop, this));
    // Create a thread for the package send loop.
    m_vThreads.push_back(std::thread(&CSERVER::packageSendLoop, this));

    loop();
}

void CSERVER::init() {
    // Create server socket
    m_iServerSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (m_iServerSocket < 0) {
        throw std::runtime_error("Failed to create socket");
    }

    // Specify the address
    sockaddr_in sainServerAddress;
    memset(&sainServerAddress, 0, sizeof(sainServerAddress));  // Zero out the structure
    sainServerAddress.sin_family = AF_INET;
    sainServerAddress.sin_port = htons(49154);

    // INADDR_ANY makes the server to listen on all interfaces
    sainServerAddress.sin_addr.s_addr = INADDR_ANY;

    // Bind the socket with the server address
    if (bind(m_iServerSocket, (struct sockaddr*)&sainServerAddress, sizeof(sainServerAddress)) < 0) {
        close(m_iServerSocket);
        throw std::runtime_error("Failed to bind socket");
    }

    std::cout << "Server IP: " << inet_ntoa(sainServerAddress.sin_addr) 
              << " on Port: " << ntohs(sainServerAddress.sin_port) << std::endl;
}

// Start the server loop.
void CSERVER::loop(){
    std::cout << "Server is running..." << std::endl;
    while(m_abRunning.load()){
        // Handle server closing logic (storing current data etc.)
    }
}

// Read packages from clients
// Used by Receive Thread
void CSERVER::packageRecLoop(){
    std::cout << "Server is recieving packages..." << std::endl;
    while (m_abRunning.load()) {
        // Recieve packages from clients
        recievePackage();
    }
}

// Write packages to clients
// Used by Send Thread
void CSERVER::packageSendLoop(){
    std::cout << "Server is sending packages..." << std::endl;
    while (m_abRunning.load()) {
        // Send packages to clients
        sendPackage();
    }
}

void CSERVER::recievePackage() {
    SPACKAGE spPackage;
    sockaddr_in saiClientAddress;
    socklen_t slClientAddressLen = sizeof(saiClientAddress);
    ssize_t recvBytes = recvfrom(m_iServerSocket, &spPackage, sizeof(SPACKAGE), 0, (struct sockaddr*)&saiClientAddress, &slClientAddressLen);
    if (recvBytes == -1) {
        perror("recvfrom failed");
        return;
    }
    if (spPackage.s_ptType == PLAYER_CONNECT) {
        std::cout << "Player connected" << std::endl;
        // Generate a new client ID
        boost::uuids::uuid uuidClientID = boost::uuids::random_generator()();
        // Only store the client address once when it is created by the server.
        m_mtx.lock();
        m_umClients[uuidClientID] = SCLIENT_INFO(false,  saiClientAddress);
        // Store the package in the map with the updated client ID.
        SPACKAGE spInitPackage(uuidClientID);
        m_umPackages[uuidClientID] = spInitPackage;
        m_mtx.unlock();

    } else if (spPackage.s_ptType == PLAYER_INFO) {
        // Add the package to the map
        m_mtx.lock();
        m_umClients[spPackage.s_uuidClientID] = SCLIENT_INFO(true,  saiClientAddress);
        m_umPackages[spPackage.s_uuidClientID] = spPackage;
        m_mtx.unlock();
    }
    else if (spPackage.s_ptType == PLAYER_DISCONNECT) {
        std::cout << "Player disconnected" << std::endl;
        // Remove the client from the map
        m_mtx.lock();
        m_umClients.erase(spPackage.s_uuidClientID);
        m_umPackages[spPackage.s_uuidClientID] = spPackage;
        m_mtx.unlock();
    }
    else {
        std::cout << "Unknown package type" << std::endl;
    }
}

void CSERVER::sendPackage() {
    if (m_umClients.size() == 0 || m_umPackages.size() == 0) {
        return;
    }
    // Have local maps so that data races are avoided.
    m_mtx.lock();
    std::unordered_map<boost::uuids::uuid, SPACKAGE> umLocalPackages = m_umPackages;
    std::unordered_map<boost::uuids::uuid, SCLIENT_INFO> umLocalClients = m_umClients;
    m_mtx.unlock();

    // Go through each package and send it to all clients.
    for (auto it = umLocalPackages.begin(); it != umLocalPackages.end(); ){
        if (it->second.s_uuidClientID == boost::uuids::nil_uuid()) {
            it++;
            continue;
        }
        if (it->second.s_ptType == PLAYER_CONNECT && umLocalClients[it->second.s_uuidClientID].s_bConnected == false) {
            std::cout << "Sending PLAYER_CONNECT package to client: " << it->second.s_uuidClientID << std::endl;
            ssize_t stSentBytes = sendto(m_iServerSocket, &it->second, sizeof(SPACKAGE), 0, (struct sockaddr*)&umLocalClients[it->second.s_uuidClientID].s_sainAddress, sizeof(umLocalClients[it->second.s_uuidClientID].s_sainAddress));
            if (stSentBytes == -1) {
                perror("sendto failed");
                return;
            }
            std::cout << "Sent PLAYER_CONNECT package to client: " << it->second.s_uuidClientID << std::endl;
            m_mtx.lock();
            m_umClients[it->second.s_uuidClientID].s_bConnected = true;
            m_mtx.unlock();
            it++;
        }
        else if (it->second.s_ptType == PLAYER_DISCONNECT) {
            for (auto const& y : umLocalClients) {
                if (!y.second.s_bConnected) continue;
                if (it->first == y.first) continue;
                std::cout << "Sending PLAYER_DISCONNECT package to client: " << y.first << std::endl;
                ssize_t stSentBytes = sendto(m_iServerSocket, &it->second, sizeof(SPACKAGE), 0, (struct sockaddr*)&y.second.s_sainAddress, sizeof(y.second.s_sainAddress));
                if (stSentBytes == -1) {
                    perror("sendto failed");
                    return;
                }
            }
            m_mtx.lock();
            m_umPackages.erase(it->first);
            m_mtx.unlock();
            it = umLocalPackages.erase(it);
        }
        else {
            for (auto const& y : umLocalClients) {
                if (!y.second.s_bConnected) continue;
                if (it->first == y.first) continue;

                ssize_t stSentBytes = sendto(m_iServerSocket, &it->second, sizeof(SPACKAGE), 0, (struct sockaddr*)&y.second.s_sainAddress, sizeof(y.second.s_sainAddress));
                if (stSentBytes == -1) {
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
void CSERVER::cleanup(){
    close(m_iServerSocket);
}

CSERVER::~CSERVER(){
    cleanup();
}
