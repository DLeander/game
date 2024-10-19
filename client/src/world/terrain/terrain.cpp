#include "terrain.h"

void CTERRAIN::init(const char* filename, int size, float fHeightScale){
    // Set the height scale
    m_fHeightScale = fHeightScale;

    // Load the height map
    loadHeightMap(filename, size);

    // Generate vertex and color data
    generateVertexData();

    // Setup the shader
    setupShader();

    // Setup the VAO, VBO
    setupBuffers();
}

bool CTERRAIN::loadHeightMap(const char* filename, int iSize) {
    FILE* file = fopen(filename, "rb");

    if (m_heightData.s_pucData != nullptr) {
        unloadHeightMap();
    }
    m_heightData.s_pucData = new unsigned char[iSize * iSize];
    std::cout << "Heightmap size: " << iSize << std::endl;
    if (m_heightData.s_pucData == nullptr) {
        std::cerr << "Could not allocate memory for height data" << std::endl;
        return false;
    }
    fread(m_heightData.s_pucData, 1, iSize * iSize, file);

    fclose(file);
    m_iSize = iSize;
    m_heightData.s_iSize= iSize;

    std::cout << "Heightmap loaded" << std::endl;
    return true;
}

bool CTERRAIN::unloadHeightMap(){
    if (m_heightData.s_pucData == nullptr) {
        delete[] m_heightData.s_pucData;
        m_heightData.s_pucData = nullptr;
        return true;
    }
    return false;
}

// Generate the vertex data for the terrain
void CTERRAIN::generateVertexData() {
    m_vVertices.clear();
    m_vColors.clear();

    for (int iZ = 0; iZ < m_iSize - 1; ++iZ) {
        for (int iX = 0; iX < m_iSize; ++iX) {
            // Vertex 1 (z row)
            m_vVertices.push_back(iX);
            m_vVertices.push_back(getScaledHeightAtPoint(iX, iZ));
            m_vVertices.push_back(iZ);
            m_vColors.push_back(getTrueHeightAtPoint(iX, iZ) / 255.0f);
            m_vColors.push_back(getTrueHeightAtPoint(iX, iZ) / 255.0f);
            m_vColors.push_back(getTrueHeightAtPoint(iX, iZ) / 255.0f);

            // Vertex 2 (z+1 row)
            m_vVertices.push_back(iX);
            m_vVertices.push_back(getScaledHeightAtPoint(iX, iZ + 1));
            m_vVertices.push_back(iZ + 1);
            m_vColors.push_back(getTrueHeightAtPoint(iX, iZ + 1) / 255.0f);
            m_vColors.push_back(getTrueHeightAtPoint(iX, iZ + 1) / 255.0f);
            m_vColors.push_back(getTrueHeightAtPoint(iX, iZ + 1) / 255.0f);
        }
    }
}

// Setup the VAO, VBO, and EBO for the terrain
void CTERRAIN::setupBuffers() {
    // Create the VAO, VBO, and EBO
    m_terrainVAO = new CVAO();
    m_terrainVAO->Bind();

    m_terrainVBO = new CVBO(m_vVertices.data(), m_vVertices.size() * sizeof(float));
    m_terrainColorVBO = new CVBO(m_vColors.data(), m_vColors.size() * sizeof(float));


    m_terrainVAO->LinkAttrib(*m_terrainVBO, 0, 3, GL_FLOAT, 0, (void*)0);
    m_terrainVAO->LinkAttrib(*m_terrainColorVBO, 1, 3, GL_FLOAT, 0, (void*)0);

    m_terrainVAO->Unbind();
    m_terrainVBO->Unbind();
    m_terrainColorVBO->Unbind();
}

void CTERRAIN::setupShader() {
    m_terrainShader = new CSHADER("resources/terrain/terrain.vert", "resources/terrain/terrain.frag");
}