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

class Client{
    public:
        Client();
        ~Client();
        void run();
    private:
        // Variables
        std::vector<std::thread> threads;
        std::mutex mtx;
        Shader* playerShader;
        // Window
        GLFWwindow* window;
        int window_height;
        int window_width;


        // Networking
        ClientNetwork network;
        std::atomic<bool> socket_active; // To indicate if the socket is active or not

        // Player
        boost::uuids::uuid clientID;
        Player* player;
        std::unordered_map<boost::uuids::uuid, Player*> otherPlayers;
        BruteForce terrain;
        Camera* camera;

        // Methods
        void init();
        void loop();
        void render();
        void renderPlayers();
        void receivePlayersInfo();
        void sendPlayerInfo();
        void renderWorld();
        static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
        static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
        static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
        static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
        void cleanup();
};