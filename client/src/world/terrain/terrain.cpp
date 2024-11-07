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

void CTERRAIN::faultFormation(int iMinDelta, int iMaxDelta, int iIterations, const int iSize, const char* saveLocation) {
    m_iSize = iSize - 32;
    // m_iSize = iSize;
    // Allocate the height map; Using float for precision
    float* cHeightMap = new float[iSize * iSize];
    // Correct the memset to initialize the entire allocated size
    std::memset(cHeightMap, 0, iSize * iSize * sizeof(float)); // Initialize to zero
    std::srand(static_cast<unsigned int>(time(0))); // Seed the random number generator

    for (int iCurrentIteration = 1; iCurrentIteration <= iIterations; iCurrentIteration++) {
        // Calculate the height for the current iteration
        int iHeight = iMaxDelta - ((iMaxDelta - iMinDelta) * iCurrentIteration) / iIterations;
        
        // Generate two random points for iRandX1, iRandX2 and iRandZ1, iRandZ2
        int iRandX1 = std::rand() % (iSize);
        int iRandZ1 = std::rand() % (iSize);
        int iRandX2 = std::rand() % (iSize);
        int iRandZ2 = std::rand() % (iSize);

        // Calculate direction vectors
        int iDirX1 = iRandX2 - iRandX1;
        int iDirZ1 = iRandZ2 - iRandZ1;

        for (int x = 0; x < iSize; x++) { // Loop to m_iSize inclusive
            for (int z = 0; z < iSize; z++) { // Loop to m_iSize inclusive
                // Vector to the current point
                int iDirX2 = x - iRandX1;
                int iDirZ2 = z - iRandZ1;

                // Check if the point is "above" the fault line
                if ((iDirX2 * iDirZ1 - iDirX1 * iDirZ2) > 0) {
                    cHeightMap[z * iSize + x] += static_cast<float>(iHeight); // Raise this point by iHeight
                }
            }
        }
    }

    // Apply filtering
    for (int row = 0; row < iSize; row++) {
        filterHeightBand(cHeightMap + row * iSize, 1, iSize, 0.6f, 8);
    }
    for (int col = 0; col < iSize; col++) {
        filterHeightBand(cHeightMap + col, iSize, iSize, 0.6f, 8);
    }

    // Find the min and max height values in the height map for normalization
    float minHeight = *std::min_element(cHeightMap, cHeightMap + iSize * iSize);
    float maxHeight = *std::max_element(cHeightMap, cHeightMap + iSize * iSize);

    // Allocate an array to hold the 8-bit grayscale data
    unsigned char* cHeightMap8bit = new unsigned char[iSize * iSize];
    // for (int i = 0; i < m_iSize*m_iSize; i++) {
    //     cHeightMap8bit[i] = static_cast<unsigned char>(255 * (cHeightMap[i] - minHeight) / (maxHeight - minHeight));
    // }

    int row = 16;
    for(int i = 0; i < m_iSize; i++){
        int col = 16;
        for(int j = 0; j < m_iSize; j++){
            cHeightMap8bit[i*m_iSize + j] = static_cast<unsigned char>(255 * (cHeightMap[row*iSize + col] - minHeight) / (maxHeight - minHeight));
            col++;
        }
        row++;
    }

    // Store the heightmap as a PNG file
    stbi_write_png(saveLocation, m_iSize, m_iSize, 1, cHeightMap8bit, m_iSize);

    // Clean up
    delete[] cHeightMap;    // Free the allocated memory for the float height map
    delete[] cHeightMap8bit; // Free the allocated memory for the 8-bit image
}

void midPointDisplacement_(float* fHeightMap, float fRoughness, float fHeight, int iSize, int iLength, glm::ivec2 v2A, glm::ivec2 v2B, glm::ivec2 v2C, glm::ivec2 v2D){
    // Recursion base case.
    if (iLength <= 1) return;
    // Calculate the midpoint
    glm::ivec2 v2Middle = glm::ivec2(v2A.y + iLength/2, v2A.x + iLength/2);
    float fAvgHeight = (fHeightMap[v2A.y*iSize + v2A.x] + fHeightMap[v2B.y*iSize + v2B.x] + fHeightMap[v2C.y*iSize + v2C.x] + fHeightMap[v2D.y*iSize + v2D.x]) / 4.0f;
    fAvgHeight += fHeight + (-fHeight + static_cast<float>(std::rand() % (static_cast<int>(2 * fHeight) + 1)));
    fHeightMap[v2Middle.y * iSize + v2Middle.x] = fAvgHeight;

    // Update fHeight
    float newHeight = fHeight * pow(2, -fRoughness);

    // Upper left quadrant
    midPointDisplacement_(fHeightMap, fRoughness, newHeight, iSize, iLength/2, v2A, glm::ivec2(v2A.x + iLength/2, v2A.y), glm::ivec2(v2A.x, v2A.y + iLength/2), v2Middle);
    // Upper right quadrant
    midPointDisplacement_(fHeightMap, fRoughness, newHeight, iSize, iLength/2, glm::ivec2(v2A.x + iLength/2, v2A.y), v2B, v2Middle, glm::ivec2(v2B.x, v2B.y+iLength/2));
    // Lower left quadrant
    midPointDisplacement_(fHeightMap, fRoughness, newHeight, iSize, iLength/2, glm::ivec2(v2A.x, v2A.y + iLength/2), v2Middle, v2C, glm::ivec2(v2C.x + iLength/2, v2C.y));
    // Lower right quadrant
    midPointDisplacement_(fHeightMap, fRoughness, newHeight, iSize, iLength/2, v2Middle, glm::ivec2(v2B.x, v2B.y + iLength/2), glm::ivec2(v2C.x + iLength/2, v2C.y), v2D);
}

void CTERRAIN::midPointDisplacement(const int iSize, const char* saveLocation){
    m_iSize = iSize;
    float* fHeightMap = new float[m_iSize * m_iSize];
    std::memset(fHeightMap, 10.0f, m_iSize * m_iSize * sizeof(float));
    std::srand(static_cast<unsigned int>(time(0)));

    midPointDisplacement_(fHeightMap, 0.6f, m_iSize, m_iSize, m_iSize, glm::ivec2(0, 0), glm::ivec2(m_iSize-1, 0), glm::ivec2(0, m_iSize-1), glm::ivec2(m_iSize-1, m_iSize-1));

    // Find the min and max height values in the height map for normalization
    float minHeight = *std::min_element(fHeightMap, fHeightMap + iSize * iSize);
    float maxHeight = *std::max_element(fHeightMap, fHeightMap + iSize * iSize);
    // Allocate an array to hold the 8-bit grayscale data
    unsigned char* cHeightMap8bit = new unsigned char[iSize * iSize];
    for(int i = 0; i < m_iSize*m_iSize; i++){
        cHeightMap8bit[i] = static_cast<unsigned char>(255 * (fHeightMap[i] - minHeight) / (maxHeight - minHeight));
    }

    // Store the heightmap as a PNG file
    stbi_write_png(saveLocation, m_iSize, m_iSize, 1, cHeightMap8bit, m_iSize);

    // Clean up
    delete[] fHeightMap;    // Free the allocated memory for the float height map
    delete[] cHeightMap8bit; // Free the allocated memory for the 8-bit image
}

void CTERRAIN::filterHeightBand(float* fpBand, int iStride, int iCount, float fFilter, int iPasses){
    float v = fpBand[0];
    int j;

    // Go through the height band and apply the erosion filter
    for (int pass = 0; pass < iPasses; pass++){
        j = iStride;
        for (int i = 0; i < iCount - 1; i++){
            fpBand[j] = fFilter * v + (1 - fFilter) * fpBand[j];
            v = fpBand[j];
            
            j += iStride;
        }
    }
}