#pragma once

// Std
#include <cstring> 
#include <iostream> 
#include <netinet/in.h> 
#include <arpa/inet.h> 
#include <sys/socket.h> 
#include <unistd.h> 
// Boost
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
// Custom
#include "common.h"

class CCLIENTNETWORK {
    private:
        int m_iClientSocket;
        boost::uuids::uuid m_uuidClientID;
        struct sockaddr_in m_sainServerAddress;
        struct sockaddr_in m_sainClientAddress;

    public:
        CCLIENTNETWORK();
        ~CCLIENTNETWORK();
        boost::uuids::uuid connect();
        void disconnect();
        void sendToServer(SPACKAGE package);
        SPACKAGE receiveFromServer();

        // Getters
        // int getClientSocket() { return client_socket; }
};