#include "EBO.h"

CEBO::CEBO(GLuint* indices, GLsizeiptr size) : m_ID(0){
    glGenBuffers(1, &m_ID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW);
}

void CEBO::Bind(){
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ID);
}

void CEBO::Unbind() {
    if (m_ID != 0) {
        GLint currentBuffer = 0;
        glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &currentBuffer);

        if (currentBuffer == static_cast<GLint>(m_ID)) {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        } else {
            std::cerr << "Warning: Unbind called, but buffer ID " << m_ID
                      << " is not currently bound." << std::endl;
        }
    }
}

void CEBO::Delete() {
    if (m_ID != 0 && glIsBuffer(m_ID)) {
        glDeleteBuffers(1, &m_ID);
        m_ID = 0;
    }
}