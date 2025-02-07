#include<iostream>
#include<glad/gl.h>
#include<GLFW/glfw3.h>
#include<stb/stb_image.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

#include "client/client.h"

GLFWwindow* start(bool bDebugMode){
    CCLIENT client(bDebugMode);
    // Run the client
    client.run();
    // return the window when the game stops.
    return client.getGLFWwindow();
}

int main(int argc, char *argv[]) {

    // Initialize GLFW
	glfwInit();
    // Tell GLFW what version of OpenGL we are using 
	// In this case we are using OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Tell GLFW we are using the CORE profile
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // Allow forward compatibility
    glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GLFW_FALSE);

    bool bDebugMode = (argc > 1 && (std::string(argv[1]) == "--debug" || std::string(argv[1]) == "-d"));
    GLFWwindow* window = start(bDebugMode);

    // This is done here at the last level so it does not interrupt deletion for other objects part of the client class.
    // This is also why start(bool bDebugMode) function is used so the scope differs.
    // Ensure GLFW window is destroyed safely
    if (window) {
        glfwDestroyWindow(window);
    }

    // Terminate GLFW
    glfwTerminate();

    return 0;
}