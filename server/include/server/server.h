#pragma once

// Std
#include<iostream>
#include <cstring> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> 
#include <unistd.h> 
#include <vector>
#include <map>
#include <thread>
#include <atomic>
#include <mutex>
#include <unordered_map>
// Boost
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
// Custom
#include "common.h"

struct clientInfo{
    bool connected;
    sockaddr_in address;

    clientInfo(){
        connected = false;
    }
    clientInfo(bool connected, sockaddr_in address){
        this->connected = connected;
        this->address = address;
    }
};

namespace std {
    template <>
    struct hash<boost::uuids::uuid> {
        size_t operator()(const boost::uuids::uuid& uuid) const {
            // Convert uuid to a string and hash the string
            std::hash<std::string> hash_fn;
            return hash_fn(to_string(uuid));
        }
    };
}

class Server{
    public:
        // Variables
        // Methods
        Server(int numClients);
        ~Server();
        void run();
    private:
        // Variables
        int serverSocket;
        std::atomic<bool> running;
        std::vector<std::thread> threads;
        std::mutex mtx;
        std::unordered_map<boost::uuids::uuid, Package> packages;
        std::unordered_map<boost::uuids::uuid, clientInfo> clients;
        // Methods
        void init();
        void loop();
        void packageRecLoop();
        void packageSendLoop();
        void cleanup();

        // Networking
        void recievePackage();
        void sendPackage();
};