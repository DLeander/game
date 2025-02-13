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
#include "collision/terrainCollision.h"

class CPLAYER {
    public:
        // Variables

        // Rendering variables
        CSHADER* m_playerShader;
        CVAO* m_playerVAO;
        CVBO* m_playerVBO;
        CEBO* m_playerEBO;

        // Player position
        glm::vec3 m_v3Position;
        glm::mat4 m_m4Model;
        float m_fSpeed;

        // Mouse movement variables
        glm::vec3 m_v3Orientation = glm::vec3(0.0f, 0.0f, -1.0f);
        glm::vec3 m_v3Up = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::vec3 m_v3Velocity = glm::vec3(0.0f, 0.0f, 0.0f);
        float m_fSensitivity = 0.005f;
        float m_fYaw = 0.0f;
        float m_fPitch = 0.0f;

        // Methods
        CPLAYER(CSHADER* shader);
        CPLAYER(CSHADER* shader, glm::mat4 m4Model);
        ~CPLAYER();
        void init();
        void checkCollisions();
        void applyGravity(float deltaTime);
        void setupModelMatrix(CCAMERA* camera);
        void setPositionFromModelMatrix();
        void draw(CCAMERA* camera);
        void drawRemote();
        void keyboard_input(GLFWwindow* window, float fDeltaTime);
        void mouse_input(GLFWwindow* window, CCAMERA* camera);
        void updateOrientation();
        bool isInitialised() { return m_bInitialised; };
        void setInitialised(bool bInitialised) { m_bInitialised = bInitialised; };
        CTERRAINCOLLISION& getTerrainCollision() { return m_terrainCollision; }

    private:
        // Variables
        bool m_bInitialised;
        int m_iIndicesSize;
        Texture* m_playerTexture;
        CTERRAINCOLLISION m_terrainCollision;

        // Movement
        bool m_bIsGrounded = false;
        float m_fGravity = -0.009f;
        float m_fJumpForce = 10.0f;

        // Methods
};