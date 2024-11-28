#include "VBO.h"

CVBO::CVBO(GLfloat* vertices, GLsizeiptr size) : m_ID(0){
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
    if (m_ID != 0) {          // Ensure the buffer has not already been deleted
        glDeleteBuffers(1, &m_ID);
        m_ID = 0;             // Reset the ID to indicate the resource is freed
    }
}