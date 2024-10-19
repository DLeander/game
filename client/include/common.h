#pragma once

#include<glad/gl.h>
#include<glm/glm.hpp>
// Boost
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

enum PackageType {
    PLAYER_CONNECT,
    PLAYER_INFO,
    PLAYER_DISCONNECT,
};

struct Package {
    PackageType type; // One indicates that its a player info package, two indicates that its a player connect package.
    boost::uuids::uuid clientID;
    glm::mat4 model;

    Package(){
        this->type = PLAYER_CONNECT;
        this->clientID = boost::uuids::nil_uuid();
        this->model = glm::mat4(1.0f);
    }
    Package(PackageType type, boost::uuids::uuid clientID, glm::mat4 model){
        this->type = type;
        this->clientID = clientID;
        this->model = model;
    }
    Package(PackageType type, boost::uuids::uuid clientID){
        this->type = type;
        this->clientID = clientID;
    }
    Package(boost::uuids::uuid clientID){
        this->type = PLAYER_CONNECT;
        this->clientID = clientID;
        this->model = glm::mat4(1.0f);
    }
};