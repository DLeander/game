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

struct playerInfo{
    glm::vec3 position;

    playerInfo(glm::vec3 position){
        this->position = position;
    }
};

class Player {
    public:
        // Variables
        Shader* playerShader;
        VAO* playerVAO;
        VBO* playerVBO;
        EBO* playerEBO;
        glm::vec3 position;
        float speed;
        // Methods
        Player(Shader* shader);
        Player(Shader* shader, playerInfo info);
        ~Player();
        void init();
        void draw(int width, int height);
        void keyboard_input(GLFWwindow* window);
        void mouse_input(GLFWwindow* window);
        bool isInitialised() { return initialised; };
        void setInitialised(bool initialised) { this->initialised = initialised; };
    private:
        // Variables
        bool initialised;
        int indices_size;

        Texture* playerTexture;

        // Methods
};