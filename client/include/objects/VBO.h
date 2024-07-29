#ifndef VBO_CLASS_H
#define VBO_CLASS_H

#include <glad/gl.h>
#include <GLFW/glfw3.h>


class VBO{
    public:
        GLuint ID;
        VBO(GLfloat* vertices, GLsizeiptr size);
        void Bind();
        void Unbind();
        void Delete();
};

#endif // VBO_CLASS_H