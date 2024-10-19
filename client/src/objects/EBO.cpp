#include "EBO.h"

CEBO::CEBO(GLuint* indices, GLsizeiptr size){
    glGenBuffers(1, &m_ID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW);
}

void CEBO::Bind(){
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ID);
}

void CEBO::Unbind(){
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void CEBO::Delete(){
    glDeleteBuffers(1, &m_ID);
}