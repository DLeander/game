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

struct SCLIENT_INFO{
    bool s_bConnected;
    sockaddr_in s_sainAddress;

    SCLIENT_INFO(){
        s_bConnected = false;
    }
    SCLIENT_INFO(bool bConnected, sockaddr_in sainAddress){
        s_bConnected = bConnected;
        s_sainAddress = sainAddress;
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

class CSERVER{
    public:
        // Variables
        // Methods
        CSERVER(int iClients);
        ~CSERVER();
        void run();
    private:
        // Variables
        int m_iServerSocket;
        std::atomic<bool> m_abRunning;
        std::vector<std::thread> m_vThreads;
        std::mutex m_mtx;
        std::unordered_map<boost::uuids::uuid, SPACKAGE> m_umPackages;
        std::unordered_map<boost::uuids::uuid, SCLIENT_INFO> m_umClients;

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