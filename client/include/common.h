#pragma once

#include<glad/gl.h>
#include<glm/glm.hpp>
// Boost
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

enum EPackageType {
    PLAYER_CONNECT,
    PLAYER_INFO,
    PLAYER_DISCONNECT,
};

struct SPACKAGE {
    EPackageType s_ptType; // One indicates that its a player info package, two indicates that its a player connect package.
    boost::uuids::uuid s_uuidClientID;
    glm::mat4 s_m4Model;

    SPACKAGE(){
        s_ptType = PLAYER_CONNECT;
        s_uuidClientID = boost::uuids::nil_uuid();
        s_m4Model = glm::mat4(1.0f);
    }
    SPACKAGE(EPackageType ptType, boost::uuids::uuid uuidClientID, glm::mat4 m4Model){
        s_ptType = ptType;
        s_uuidClientID = uuidClientID;
        s_m4Model = m4Model;
    }
    SPACKAGE(EPackageType ptType, boost::uuids::uuid uuidClientID){
        s_ptType = ptType;
        s_uuidClientID = uuidClientID;
        s_m4Model = glm::mat4(1.0f);
    }
    SPACKAGE(boost::uuids::uuid uuidClientID){
        s_ptType = PLAYER_CONNECT;
        s_uuidClientID = uuidClientID;
        s_m4Model = glm::mat4(1.0f);
    }
};