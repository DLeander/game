#pragma once

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

#include "shader/shaderClass.h"

class Camera {
    public:
        // Variables
        glm::vec3 position;
        glm::vec3 orientation = glm::vec3(0.0f, 0.0f, -1.0f);
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
        
        int width;
        int height;

        float speed = 0.1f;
        float sensitivity = 100.0f;

        // Methods
        Camera(int width, int height, glm::vec3 position);
        
        void matrix(float FOVdeg, float nearPlane, float farPlane, Shader* shader, const char* uniform);
        void inputs(GLFWwindow* window);
    private:
        // Variables
        glm::mat4 view;
        glm::mat4 projection;
        glm::vec2 oldMousePosition;
        bool firstClick = true;
        // Methods
        void updateCameraVectors();
};