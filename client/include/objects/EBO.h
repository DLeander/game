#pragma once

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <cassert>
#include <iostream>


class CEBO{
    public:
        GLuint m_ID;
        CEBO(GLuint* indices, GLsizeiptr size);
        // ~CEBO() {Delete();}
        ~CEBO(){};
        void Bind();
        void Unbind();
        void Delete();
};