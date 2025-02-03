#pragma once

// Std
#include <cstring> 
#include <iostream> 
#include <map>
#include <vector>
#include <thread>
#include <mutex>
#include <unordered_map>
#include <atomic>
// glad
#include<glad/gl.h>
// GLFW
#include<GLFW/glfw3.h>
// STB
#include<stb/stb_image.h>
// GLM
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
// Boost
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
// Custom
#include "shader/shaderClass.h"
#include "objects/VAO.h"
#include "objects/VBO.h"
#include "objects/EBO.h"
#include "player.h"
#include "common.h"
#include "client_network.h"
#include "camera.h"
#include "terrain.h"
#include "bruteforce.h"
#include "geomipmap.h"

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

class CCLIENT{
    public:
        CCLIENT(bool bDoOfflineMode = false);
        ~CCLIENT();
        void run();
    private:
        // Variables
        std::vector<std::thread> m_vThreads;
        std::mutex m_mtx;
        CSHADER* m_playerShader;
        // Window
        GLFWwindow* m_window;
        int m_iWindowHeight;
        int m_iWindowWidth;


        // Networking
        CCLIENTNETWORK m_network;
        std::atomic<bool> m_abSocketActive; // To indicate if the socket is active or not
        bool m_bDoOfflineMode = false;

        // Player
        boost::uuids::uuid m_uuidClientID;
        CPLAYER* m_player;
        std::unordered_map<boost::uuids::uuid, CPLAYER*> m_umOtherPlayers;
        CGEOMIPMAP m_terrain;
        CCAMERA* m_camera;

        // Methods
        void init();
        void loop();
        void render();
        void renderPlayers();
        void receivePlayersInfo();
        void sendPlayerInfo();
        void renderWorld();
        static void mouse_callback(GLFWwindow* window, double dXpos, double dYpos);
        static void scroll_callback(GLFWwindow* window, double dXoffset, double dYoffset);
        static void key_callback(GLFWwindow* window, int iKey, int iScancode, int iAction, int iMods);
        static void mouse_button_callback(GLFWwindow* window, int iButton, int iAction, int mods);
        void cleanup();
};