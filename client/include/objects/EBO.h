#pragma once

#include <glad/gl.h>
#include <GLFW/glfw3.h>


class CEBO{
    public:
        GLuint m_ID;
        CEBO(GLuint* indices, GLsizeiptr size);
        void Bind();
        void Unbind();
        void Delete();
};