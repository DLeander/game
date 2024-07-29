#include<iostream>
#include<glad/gl.h>
#include<GLFW/glfw3.h>
#include<stb/stb_image.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

#include "client/client.h"

int main(int argc, char *argv[]){   
    Client client;
    client.run();
    return 0;
}