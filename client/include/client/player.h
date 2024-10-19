#pragma once

#include <cstring> 
#include <iostream> 
#include <netinet/in.h> 
#include <sys/socket.h> 
#include <unistd.h> 

#include<glad/gl.h>
#include<GLFW/glfw3.h>
#include<stb/stb_image.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

#include "shader/shaderClass.h"
#include "objects/VAO.h"
#include "objects/VBO.h"
#include "objects/EBO.h"
#include "texture/texture.h"
#include "camera.h"

class Player {
    public:
        // Variables

        // Rendering variables
        Shader* playerShader;
        VAO* playerVAO;
        VBO* playerVBO;
        EBO* playerEBO;

        // Player position
        glm::vec3 position;
        glm::mat4 model;
        float speed;

        // Mouse movement variables
        glm::vec3 orientation = glm::vec3(0.0f, 0.0f, -1.0f);
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
        float sensitivity = 0.005f;
        float yaw = 0.0f;
        float pitch = 0.0f;

        // Methods
        Player(Shader* shader);
        Player(Shader* shader, glm::mat4 model);
        ~Player();
        void init();
        void setupModelMatrix(Camera* camera);
        void setPositionFromModelMatrix();
        void draw(Camera* camera);
        void drawRemote();
        void keyboard_input(GLFWwindow* window);
        void mouse_input(GLFWwindow* window, Camera* camera);
        void updateOrientation();
        bool isInitialised() { return initialised; };
        void setInitialised(bool initialised) { this->initialised = initialised; };
    private:
        // Variables
        bool initialised;
        int indices_size;

        Texture* playerTexture;

        glm::vec2 oldMousePosition;
        bool firstClick = true;

        // Methods
};