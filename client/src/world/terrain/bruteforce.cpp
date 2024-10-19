#include "bruteforce.h"

void CBRUTEFORCE::render(CCAMERA* camera) {
    // Activate shader program
    m_terrainShader->Activate();  // Ensure shader is correctly set up
    camera->matrix(45.0f, 0.1f, 300.0f, m_terrainShader, "camMatrix");

    GLint location = glGetUniformLocation(m_terrainShader->m_ID, "camMatrix");
    if (location == -1) {
        std::cerr << "Uniform 'camMatrix' not found in shader." << std::endl;
    }

    // Bind the VAO that stores vertex and color data
    m_terrainVAO->Bind();

    // Draw the terrain using the vertex data and color data
    // glDrawArrays(GL_TRIANGLE_STRIP, 0, (vertices.size() / 3));

    // Draw each row separately
    for (int iZ = 0; iZ < m_iSize - 1; ++iZ) {
        // Calculate the offset into the vertex array
        int offset = iZ * m_iSize * 2 * 3; // 2 vertices per x, 3 components per vertex
        glDrawArrays(GL_TRIANGLE_STRIP, offset / 3, m_iSize * 2); // size * 2 vertices per row
    }

    // Unbind the VAO
    m_terrainVAO->Unbind();
}