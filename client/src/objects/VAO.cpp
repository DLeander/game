#include "VAO.h"

CVAO::CVAO(){
    glGenVertexArrays(1, &m_ID);
}

void CVAO::LinkAttrib(CVBO& VBO, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset){
    VBO.Bind();
    glVertexAttribPointer(layout, numComponents, type, GL_FALSE, stride, offset);
    glEnableVertexAttribArray(layout);
    VBO.Unbind();
}

void CVAO::Bind(){
    glBindVertexArray(m_ID);
}

void CVAO::Unbind(){
    glBindVertexArray(0);
}
void CVAO::Delete(){
    glDeleteVertexArrays(1, &m_ID);
}