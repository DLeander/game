#pragma once

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

#include "shader/shaderClass.h"

class CCAMERA {
    public:
        // Variables
        glm::vec3 m_v3Position;
        glm::vec3 m_v3Orientation = glm::vec3(0.0f, 0.0f, -1.0f);
        glm::vec3 m_v3Up = glm::vec3(0.0f, 1.0f, 0.0f);

        glm::mat4 m_m4View;
        glm::mat4 m_m4Projection;
        
        int m_iWidth;
        int m_iHeight;

        float m_fSpeed = 0.001f;
        float m_fSensitivity = 1.0f;

        // Methods
        CCAMERA(int iWidth, int iHeight, glm::vec3 v3Position);
        void matrix(float fFOVdeg, float fNearPlane, float fFarPlane, CSHADER* shader, const char* uniform);
        void updateCameraOrientation(float fYaw, float fPitch);
        // void matrix(float FOVdeg, float nearPlane, float farPlane, Shader* shader, glm::mat4 model);
        // void inputs(GLFWwindow* window);

    private:
        // glm::vec2 oldMousePosition;
        // bool firstClick = true;
        // Methods
};