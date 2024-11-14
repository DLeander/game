#pragma once

#include <glad/gl.h>
#include <stb/stb_image.h>

#include "shaderClass.h"

class Texture {
    public:
        // Variables
        GLuint m_ID;
        GLenum m_Type;

        int m_iWidth, m_iHeight, m_iChannels;

        // Methods
        Texture(const char* texturePath, GLenum type, GLenum slot, GLenum format, GLenum pixelType);
        Texture(unsigned char* texture, GLenum type, GLenum slot, GLenum format, GLenum pixelType);

        void textureUnit(CSHADER* shader, const char* uniform, GLuint unit);
        void Bind();
        void Unbind();
        void Delete();
};