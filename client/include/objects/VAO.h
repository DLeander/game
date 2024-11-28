#pragma once

#include <iostream> 

#include <glad/gl.h>
#include "VBO.h"

class CVAO{
    public:
        GLuint m_ID;
        CVAO();
        // ~CVAO() {Delete();}
        ~CVAO(){};
        void LinkAttrib(CVBO& VBO, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset);
        void Bind();
        void Unbind();
        void Delete();
};