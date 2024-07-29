#include "client_network.h"

ClientNetwork::ClientNetwork(){

}

ClientNetwork::~ClientNetwork(){
    disconnect();
}

// Set the socket to non-blocking if socket is about to be disconnect or a connection retry is needed.
void setReceiveTimeout(int socket, int timeout_ms) {
    struct timeval timeout;
    timeout.tv_sec = timeout_ms / 1000;
    timeout.tv_usec = (timeout_ms % 1000) * 1000;
    if (setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
        perror("setsockopt");
        throw std::runtime_error("Failed to set receive timeout");
    }
}

boost::uuids::uuid ClientNetwork::connect() {
    // Initialize connection to server
    client_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (client_socket < 0) {
        perror("Failed to create socket");
        return boost::uuids::nil_uuid();
    }

    // Zero out the structures
    memset(&server_address, 0, sizeof(server_address));
    memset(&client_address, 0, sizeof(client_address));

    // Specify server address
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8080); 
    if (inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr) <= 0) {
        perror("inet_pton failed");
        close(client_socket);
        return boost::uuids::nil_uuid();
    }

    // Specify client address
    client_address.sin_family = AF_INET;
    client_address.sin_addr.s_addr = INADDR_ANY;
    client_address.sin_port = htons(0); // Bind to any available port

    if (bind(client_socket, (struct sockaddr*)&client_address, sizeof(client_address)) == -1) {
        perror("Bind failed");
        close(client_socket);
        return boost::uuids::nil_uuid();
    }

    // Ask server for client ID 
    Package initPackage;
    setReceiveTimeout(client_socket, 100);
    unsigned int retries = 0;
    while (retries < 25){
        if (retries > 0){
            std::cout << "Retrying to connect to server" << std::endl;
        }
        sendToServer(initPackage);
        std::cout << "Asking server for client ID" << std::endl;
        initPackage = receiveFromServer();
        if (initPackage.clientID != boost::uuids::nil_uuid()){
            break;
        }
        retries++;
    }
    if (retries == 25){
        std::cout << "Failed to connect to server" << std::endl;
        close(client_socket);
        return boost::uuids::nil_uuid();
    }
    std::cout << "Created client with uuid: " << initPackage.clientID  << "after " << retries << " retires." << std::endl;

    clientID = initPackage.clientID;
    return clientID;
}

void ClientNetwork::disconnect(){
    // Send the client ID to the server to disconnect.
    Package disconnectPackage(PLAYER_DISCONNECT, clientID, glm::vec3(0.0f, 0.0f, 0.0f));
    sendToServer(disconnectPackage);
    // Wait for the server to disconnect the client
    // std::this_thread::sleep_for(std::chrono::milliseconds(100));
    // Close the socket
    // close(client_socket);
}

void ClientNetwork::sendToServer(Package package){
    sendto(client_socket, &package, sizeof(package), 0, (struct sockaddr*)&server_address, sizeof(server_address));
}

Package ClientNetwork::receiveFromServer() {
    Package package;
    socklen_t serverAddrLen = sizeof(server_address);
    ssize_t recvBytes = recvfrom(client_socket, &package, sizeof(Package), 0, (struct sockaddr*)&server_address, &serverAddrLen);
    if (recvBytes == -1) {
        if (errno == EWOULDBLOCK || errno == EAGAIN) {
            // Timeout occurred, continue the loop
            return package;
        } 
        else if (errno == ENETDOWN || errno == ENETUNREACH || errno == ENETRESET) {
            // Network is down, unreachable, or reset
            perror("recvfrom: Network error");
            throw std::runtime_error("Network error: Network is down, unreachable, or reset");
        }
        else {
            // Other errors
            perror("recvfrom");
            throw std::runtime_error("Failed to receive package from server");
        }
    }
    return package;
}