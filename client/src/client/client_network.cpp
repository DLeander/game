#include "client_network.h"

CCLIENTNETWORK::CCLIENTNETWORK(){

}

CCLIENTNETWORK::~CCLIENTNETWORK(){
    disconnect();
}

// Set the socket to non-blocking if socket is about to be disconnect or a connection retry is needed.
void setReceiveTimeout(int iSocket, int iTimeout_ms) {
    struct timeval stvTimeout;
    stvTimeout.tv_sec = iTimeout_ms / 1000;
    stvTimeout.tv_usec = (iTimeout_ms % 1000) * 1000;
    if (setsockopt(iSocket, SOL_SOCKET, SO_RCVTIMEO, &stvTimeout, sizeof(stvTimeout)) < 0) {
        perror("setsockopt");
        throw std::runtime_error("Failed to set receive timeout");
    }
}

boost::uuids::uuid CCLIENTNETWORK::connect() {
    // Initialize connection to server
    m_iClientSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (m_iClientSocket < 0) {
        perror("Failed to create socket"); 
        return boost::uuids::nil_uuid();
    }

    // Zero out the structures
    memset(&m_sainServerAddress, 0, sizeof(m_sainServerAddress));
    memset(&m_sainClientAddress, 0, sizeof(m_sainClientAddress));

    // Specify server address
    m_sainServerAddress.sin_family = AF_INET;
    m_sainServerAddress.sin_port = htons(49154); 
    // Localhost
    // 127.0.0.1
    // "90.225.101.50"
    if (inet_pton(AF_INET, "127.0.0.1", &m_sainServerAddress.sin_addr) <= 0) {
        perror("inet_pton failed");
        close(m_iClientSocket);
        return boost::uuids::nil_uuid();
    }

    // Specify client address
    m_sainClientAddress.sin_family = AF_INET;
    m_sainClientAddress.sin_addr.s_addr = INADDR_ANY;
    m_sainClientAddress.sin_port = htons(0); // Bind to any available port

    if (bind(m_iClientSocket, (struct sockaddr*)&m_sainClientAddress, sizeof(m_sainClientAddress)) == -1) {
        perror("Bind failed");
        close(m_iClientSocket);
        return boost::uuids::nil_uuid();
    }

    // Ask server for client ID 
    SPACKAGE initPackage;
    setReceiveTimeout(m_iClientSocket, 100);
    unsigned int uiRetries = 0;
    while (uiRetries < 25){
        if (uiRetries > 0){
            std::cout << "Retrying to connect to server" << std::endl;
        }
        sendToServer(initPackage);
        std::cout << "Asking server for client ID" << std::endl;
        initPackage = receiveFromServer();
        if (initPackage.s_uuidClientID != boost::uuids::nil_uuid()){
            break;
        }
        uiRetries++;
    }
    if (uiRetries == 25){
        std::cout << "Failed to connect to server" << std::endl;
        close(m_iClientSocket);
        return boost::uuids::nil_uuid();
    }
    std::cout << "Created client with uuid: " << initPackage.s_uuidClientID  << "after " << uiRetries << " retires." << std::endl;

    m_uuidClientID = initPackage.s_uuidClientID;
    return m_uuidClientID;
}

void CCLIENTNETWORK::disconnect(){
    // Send the client ID to the server to disconnect.
    SPACKAGE disconnectPackage(PLAYER_DISCONNECT, m_uuidClientID);
    sendToServer(disconnectPackage);
    // Wait for the server to disconnect the client
    // std::this_thread::sleep_for(std::chrono::milliseconds(100));
    // Close the socket
    // close(client_socket);
}

void CCLIENTNETWORK::sendToServer(SPACKAGE package){
    sendto(m_iClientSocket, &package, sizeof(package), 0, (struct sockaddr*)&m_sainServerAddress, sizeof(m_sainServerAddress));
}

SPACKAGE CCLIENTNETWORK::receiveFromServer() {
    SPACKAGE package;
    socklen_t slServerAddrLen = sizeof(m_sainServerAddress);
    ssize_t recvBytes = recvfrom(m_iClientSocket, &package, sizeof(SPACKAGE), 0, (struct sockaddr*)&m_sainServerAddress, &slServerAddrLen);
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