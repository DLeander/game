#include "bruteforce.h"

void BruteForce::Render(Camera* camera) {
    // Activate shader program
    terrainShader->Activate();  // Ensure shader is correctly set up
    camera->matrix(45.0f, 0.1f, 300.0f, terrainShader, "camMatrix");

    GLint location = glGetUniformLocation(terrainShader->ID, "camMatrix");
    if (location == -1) {
        std::cerr << "Uniform 'camMatrix' not found in shader." << std::endl;
    }

    // Bind the VAO that stores vertex and color data
    terrainVAO->Bind();

    // Draw the terrain using the vertex data and color data
    // glDrawArrays(GL_TRIANGLE_STRIP, 0, (vertices.size() / 3));

    // Draw each row separately
    for (int z = 0; z < size - 1; ++z) {
        // Calculate the offset into the vertex array
        int offset = z * size * 2 * 3; // 2 vertices per x, 3 components per vertex
        glDrawArrays(GL_TRIANGLE_STRIP, offset / 3, size * 2); // size * 2 vertices per row
    }

    // Unbind the VAO
    terrainVAO->Unbind();
}