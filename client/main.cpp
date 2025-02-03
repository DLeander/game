#include<iostream>
#include<glad/gl.h>
#include<GLFW/glfw3.h>
#include<stb/stb_image.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

#include "client/client.h"

int main(int argc, char *argv[]){
    if (argc > 1){
        if (std::string(argv[1]) == "-debug"){
            CCLIENT client(true);
            client.run();
        }
    }
    else{
        CCLIENT client;
        client.run();
        return 0;
    }
    return 0;
}