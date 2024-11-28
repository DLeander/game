#pragma once

#include <glad/gl.h>
#include <GLFW/glfw3.h>


class CVBO{
    public:
        GLuint m_ID;
        CVBO(GLfloat* vertices, GLsizeiptr size);
        // ~CVBO() {Delete();}
        ~CVBO(){};
        void Bind();
        void Unbind();
        void Delete();
};