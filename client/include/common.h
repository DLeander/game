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
    glm::vec3 position;

    Package(){
        type = PLAYER_CONNECT;
        clientID = boost::uuids::nil_uuid();
        position = glm::vec3(0.0f, 0.0f, 0.0f);
    }
    Package(PackageType type, boost::uuids::uuid clientID, glm::vec3 position){
        this->type = type;
        this->clientID = clientID;
        this->position = position;
    }
    Package(boost::uuids::uuid clientID){
        this->type = PLAYER_CONNECT;
        this->clientID = clientID;
        this->position = glm::vec3(0.0f, 0.0f, 0.0f);
    }
};