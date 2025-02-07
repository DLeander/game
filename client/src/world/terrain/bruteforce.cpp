#include "bruteforce.h"

CBRUTEFORCE::~CBRUTEFORCE(){
        if (m_terrainTexCoordsVBO){m_terrainTexCoordsVBO->Delete(); delete m_terrainTexCoordsVBO; m_terrainTexCoordsVBO = nullptr;}
        if (m_terrainVAO){m_terrainVAO->Delete(); delete m_terrainVAO; m_terrainVAO = nullptr;}
        if (m_terrainVBO){m_terrainVBO->Delete(); delete m_terrainVBO; m_terrainVBO = nullptr;}
        if (m_terrainColorVBO){m_terrainColorVBO->Delete(); delete m_terrainColorVBO; m_terrainColorVBO = nullptr;}
    }

void CBRUTEFORCE::render(CCAMERA* camera) {
    // Activate shader program
    m_terrainShader->Activate();  // Ensure shader is correctly set up
    camera->matrix(45.0f, 0.1f, 300.0f, m_terrainShader, "camMatrix");

    GLint location = glGetUniformLocation(m_terrainShader->m_ID, "camMatrix");
    if (location == -1) {
        std::cerr << "Uniform 'camMatrix' not found in shader." << std::endl;
    }

    // Set the model matrix
    m_terrainTexture->Bind();
    // Bind the VAO that stores vertex and color data
    m_terrainVAO->Bind();

    // Draw each row separately
    for (int iZ = 0; iZ < m_iSize - 1; ++iZ) {
        // Calculate the offset into the vertex array
        int offset = iZ * m_iSize * 2 * 3; // 2 vertices per x, 3 components per vertex
        glDrawArrays(GL_TRIANGLE_STRIP, offset / 3, m_iSize * 2); // size * 2 vertices per row
    }

    // Unbind the VAO
    m_terrainVAO->Unbind();
}

// Setup the VAO, VBO, and EBO for the terrain
void CBRUTEFORCE::setupBuffers() {

    m_vVertices.clear();
    m_vColors.clear();

    for (int iZ = 0; iZ < m_iSize - 1; ++iZ) {
        for (int iX = 0; iX < m_iSize; ++iX) {
            
            // Textures are stored for x and z, and x and z + 1 (depending no the currently drawn terrain.)
            // These can be called TextureLeft, TextureBottom, and TextureTop.

            // Vertex 1 (z row)
            m_vVertices.push_back(iX);
            m_vVertices.push_back(getScaledHeightAtPoint(iX, iZ));
            m_vVertices.push_back(iZ);
            m_vColors.push_back(getTrueHeightAtPoint(iX, iZ) / 255.0f);
            m_vColors.push_back(getTrueHeightAtPoint(iX, iZ) / 255.0f);
            m_vColors.push_back(getTrueHeightAtPoint(iX, iZ) / 255.0f);
            m_vTexCoords.push_back(static_cast<float>(iX) / (m_iSize)); // textureLeft
            m_vTexCoords.push_back(static_cast<float>(iZ) / (m_iSize)); // textureBottom

            // Vertex 2 (z+1 row)
            m_vVertices.push_back(iX);
            m_vVertices.push_back(getScaledHeightAtPoint(iX, iZ + 1));
            m_vVertices.push_back(iZ + 1);
            m_vColors.push_back(getTrueHeightAtPoint(iX, iZ + 1) / 255.0f);
            m_vColors.push_back(getTrueHeightAtPoint(iX, iZ + 1) / 255.0f);
            m_vColors.push_back(getTrueHeightAtPoint(iX, iZ + 1) / 255.0f);
            m_vTexCoords.push_back(static_cast<float>(iX) / (m_iSize)); // textureLeft
            m_vTexCoords.push_back(static_cast<float>(iZ + 1) / (m_iSize)); // textureTop
        }
    }

    // Create the VAO, VBO, and EBO
    m_terrainVAO = new CVAO();
    m_terrainVAO->Bind();

    m_terrainVBO = new CVBO(m_vVertices.data(), m_vVertices.size() * sizeof(float));
    m_terrainColorVBO = new CVBO(m_vColors.data(), m_vColors.size() * sizeof(float));
    m_terrainTexCoordsVBO = new CVBO(m_vTexCoords.data(), m_vTexCoords.size() * sizeof(float));

    m_terrainVAO->LinkAttrib(*m_terrainVBO, 0, 3, GL_FLOAT, 0, (void*)0);
    m_terrainVAO->LinkAttrib(*m_terrainColorVBO, 1, 3, GL_FLOAT, 0, (void*)0);
    m_terrainVAO->LinkAttrib(*m_terrainTexCoordsVBO, 2, 2, GL_FLOAT, 0, (void*)0);

    m_terrainVAO->Unbind();
    m_terrainVBO->Unbind();
    m_terrainColorVBO->Unbind();
    m_terrainTexCoordsVBO->Unbind();
}

void CBRUTEFORCE::createTextureFromHeightMap() {
    // Define RGB colors for green (low altitude) and gray (high altitude)
    unsigned char green[3] = {65, 155, 55}; // Grass color
    unsigned char gray[3] = {81, 81, 81};   // Rock color

    // Allocate memory for the color map
    unsigned char* ucColorMap = new unsigned char[m_iSize * m_iSize * 3];

    for (int i = 0; i < m_iSize * m_iSize; i++) {
        // Normalize the height value (0 to 1 range)
        float fHeight = static_cast<float>(m_heightData.s_pucData[i]) / 255.0f;
        
        // Interpolate between colors based on height (low = green, high = gray)
        unsigned char ucR = static_cast<unsigned char>(fHeight * gray[0] + (1.0f - fHeight)/2 * green[0]);
        unsigned char ucG = static_cast<unsigned char>(fHeight * gray[1] + (1.0f - fHeight)/2 * green[1]);
        unsigned char ucB = static_cast<unsigned char>(fHeight * gray[2] + (1.0f - fHeight)/2 * green[2]);
        if (i*3 < m_iSize * m_iSize * 3){
            setColorToTexture(ucColorMap, i*3, ucR, ucG, ucB);
        }
    }

    m_ucTextureData = ucColorMap;
}