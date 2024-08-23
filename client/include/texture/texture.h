#pragma once

#include <glad/gl.h>
#include <stb/stb_image.h>

#include "shaderClass.h"

class Texture {
    public:
        // Variables
        GLuint ID;
        GLenum type;

        int width, height, nrChannels;

        // Methods
        Texture(const char* texturePath, GLenum type, GLenum slot, GLenum format, GLenum pixelType);

        void textureUnit(Shader* shader, const char* uniform, GLuint unit);
        void Bind();
        void Unbind();
        void Delete();
};