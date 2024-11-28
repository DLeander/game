#include "VAO.h"

CVAO::CVAO() : m_ID(0){
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
    if (m_ID != 0) {          // Ensure the buffer has not already been deleted
        glDeleteVertexArrays(1, &m_ID);
        m_ID = 0;             // Reset the ID to indicate the resource is freed
    }
    
}