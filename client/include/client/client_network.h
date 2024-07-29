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

class ClientNetwork {
    private:
        int client_socket;
        boost::uuids::uuid clientID;
        struct sockaddr_in server_address;
        struct sockaddr_in client_address;

    public:
        ClientNetwork();
        ~ClientNetwork();
        boost::uuids::uuid connect();
        void disconnect();
        void sendToServer(Package package);
        Package receiveFromServer();

        // Getters
        // int getClientSocket() { return client_socket; }
};