#include "VBO.h"

CVBO::CVBO(GLfloat* vertices, GLsizeiptr size){
    glGenBuffers(1, &m_ID);
    glBindBuffer(GL_ARRAY_BUFFER, m_ID);
    glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
}

void CVBO::Bind(){
    glBindBuffer(GL_ARRAY_BUFFER, m_ID);
}

void CVBO::Unbind(){
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void CVBO::Delete(){
    glDeleteBuffers(1, &m_ID);
}