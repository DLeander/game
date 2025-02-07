#include "terrain.h"

CTERRAIN::~CTERRAIN(){
    unloadHeightMap();
    if (m_terrainShader){m_terrainShader->Delete(); delete m_terrainShader; m_terrainShader = nullptr;}
    if (m_terrainTexture){m_terrainTexture->Delete(); delete m_terrainTexture; m_terrainTexture = nullptr;}
    if (m_ucTextureData){delete[] m_ucTextureData; m_ucTextureData = nullptr;}
}

void CTERRAIN::init(const char* filename, int size, float fHeightScale){
    // Set the height scale
    m_fHeightScale = fHeightScale;

    // Load the height map
    loadHeightMap(filename, size);
    setupShader();
    setupBuffers();

    // Setup the texture
    createTextureFromHeightMap();

    // The different x and z directions we can have in slope lightning is
    // 1, 1 (45 degrees), 1, 0 (90 degrees), 1, -1 (135 degrees), 0, -1 (180 degrees), -1, -1 (225 degrees), -1, 0 (270 degrees), -1, 1 (315 degrees), 0, 1 (360 degrees)
    // The sun will always move in 45 degree increments, so the day/night cycle should only recalculate the lightning every "45 degrees" of suntime.
    setSlopeLightingParams(1, 1, 0.1f, 1.0f, 15.0f);
    calculateLightning();
    setupTexture();

    std::cout << "Terrain initialized" << std::endl;
}

bool CTERRAIN::loadHeightMap(const char* filename, int iSize) {
    FILE* file = fopen(filename, "rb");

    if (m_heightData.s_pucData != nullptr) {
        unloadHeightMap();
    }
    m_heightData.s_pucData = new unsigned char[iSize * iSize];
    if (m_heightData.s_pucData == nullptr) {
        std::cerr << "Could not allocate memory for height data" << std::endl;
        return false;
    }
    fread(m_heightData.s_pucData, 1, iSize * iSize, file);

    fclose(file);
    m_iSize = iSize;
    m_heightData.s_iSize= iSize;

    return true;
}

bool CTERRAIN::unloadHeightMap(){
    if (m_heightData.s_pucData != nullptr) {
        delete[] m_heightData.s_pucData;
        m_heightData.s_pucData = nullptr;
        return true;
    }
    return false;
}

// void CTERRAIN::setupTexture(const char* filename) {
//     m_terrainTexture = new Texture(filename, GL_TEXTURE_2D, GL_TEXTURE0, GL_RGB, GL_UNSIGNED_BYTE);
//     m_terrainTexture->textureUnit(m_terrainShader, "texture1", 0);
// }

void CTERRAIN::setupTexture() {
    if (m_terrainTexture) {
        std::cerr << "Texture existed already" << std::endl;
        m_terrainTexture->Delete();
        delete m_terrainTexture;
        m_terrainTexture = nullptr;
    }
    m_terrainTexture = new Texture(m_ucTextureData, GL_TEXTURE_2D, GL_TEXTURE0, GL_RGB, GL_UNSIGNED_BYTE, m_iSize, m_iSize);
    m_terrainTexture->textureUnit(m_terrainShader, "texture1", 0);
}

void CTERRAIN::setupShader() {
    m_terrainShader = new CSHADER("resources/terrain/terrain.vert", "resources/terrain/terrain.frag");
}

void CTERRAIN::setColorToTexture(unsigned char* ucTexture, int iIndex, unsigned char ucR, unsigned char ucG, unsigned char ucB) {
    ucTexture[iIndex] = ucR;
    ucTexture[iIndex + 1] = ucG;
    ucTexture[iIndex + 2] = ucB;
}

// Current implementation is slope lightning.
// This might change to a better lightning later on.
// This function should be called after setSlopeLightningParams
// And each time the light should change (aka day/night cycle).
void CTERRAIN::calculateLightning(){
    float fShade;
    for (int z = 0; z < m_iSize; z++){
        for (int x = 0; x < m_iSize; x++){
            if(z >= m_iLightDirectionZ && x >= m_iLightDirectionX){
                //calculate the shading value using the “slope lighting” algorithm
                fShade= 1.0f-( getTrueHeightAtPoint( x-m_iLightDirectionX, z-m_iLightDirectionZ ) - getTrueHeightAtPoint( x, z ) )/m_fLightSoftness;
            }else{
                fShade= 1.0f;
            }

            if (fShade < m_fMinLightBrightness) fShade = m_fMinLightBrightness;
            if (fShade > m_fMaxLightBrightness) fShade = m_fMaxLightBrightness;
            setColorToTexture(m_ucTextureData, (z*m_iSize + x)*3, m_ucTextureData[(z*m_iSize + x)*3]*fShade, m_ucTextureData[(z*m_iSize + x)*3+1]*fShade, m_ucTextureData[(z*m_iSize + x)*3+2]*fShade);
        }
    }
    // setupTexture();
}

// void CTERRAIN::faultFormation(int iMinDelta, int iMaxDelta, int iIterations, const int iSize, const char* saveLocation) {
//     m_iSize = iSize - 32;
//     // m_iSize = iSize;
//     // Allocate the height map; Using float for precision
//     float* cHeightMap = new float[iSize * iSize];
//     // Correct the memset to initialize the entire allocated size
//     std::memset(cHeightMap, 0, iSize * iSize * sizeof(float)); // Initialize to zero
//     std::srand(static_cast<unsigned int>(time(0))); // Seed the random number generator

//     for (int iCurrentIteration = 1; iCurrentIteration <= iIterations; iCurrentIteration++) {
//         // Calculate the height for the current iteration
//         int iHeight = iMaxDelta - ((iMaxDelta - iMinDelta) * iCurrentIteration) / iIterations;
        
//         // Generate two random points for iRandX1, iRandX2 and iRandZ1, iRandZ2
//         int iRandX1 = std::rand() % (iSize);
//         int iRandZ1 = std::rand() % (iSize);
//         int iRandX2 = std::rand() % (iSize);
//         int iRandZ2 = std::rand() % (iSize);

//         // Calculate direction vectors
//         int iDirX1 = iRandX2 - iRandX1;
//         int iDirZ1 = iRandZ2 - iRandZ1;

//         for (int x = 0; x < iSize; x++) { // Loop to m_iSize inclusive
//             for (int z = 0; z < iSize; z++) { // Loop to m_iSize inclusive
//                 // Vector to the current point
//                 int iDirX2 = x - iRandX1;
//                 int iDirZ2 = z - iRandZ1;

//                 // Check if the point is "above" the fault line
//                 if ((iDirX2 * iDirZ1 - iDirX1 * iDirZ2) > 0) {
//                     cHeightMap[z * iSize + x] += static_cast<float>(iHeight); // Raise this point by iHeight
//                 }
//             }
//         }
//     }

//     // Apply filtering
//     for (int row = 0; row < iSize; row++) {
//         filterHeightBand(cHeightMap + row * iSize, 1, iSize, 0.6f, 8);
//     }
//     for (int col = 0; col < iSize; col++) {
//         filterHeightBand(cHeightMap + col, iSize, iSize, 0.6f, 8);
//     }

//     // Find the min and max height values in the height map for normalization
//     float minHeight = *std::min_element(cHeightMap, cHeightMap + iSize * iSize);
//     float maxHeight = *std::max_element(cHeightMap, cHeightMap + iSize * iSize);

//     // Allocate an array to hold the 8-bit grayscale data
//     unsigned char* cHeightMap8bit = new unsigned char[iSize * iSize];
//     // for (int i = 0; i < m_iSize*m_iSize; i++) {
//     //     cHeightMap8bit[i] = static_cast<unsigned char>(255 * (cHeightMap[i] - minHeight) / (maxHeight - minHeight));
//     // }

//     int row = 16;
//     for(int i = 0; i < m_iSize; i++){
//         int col = 16;
//         for(int j = 0; j < m_iSize; j++){
//             cHeightMap8bit[i*m_iSize + j] = static_cast<unsigned char>(255 * (cHeightMap[row*iSize + col] - minHeight) / (maxHeight - minHeight));
//             col++;
//         }
//         row++;
//     }

//     // Store the heightmap as a PNG file
//     stbi_write_png(saveLocation, m_iSize, m_iSize, 1, cHeightMap8bit, m_iSize);

//     // Clean up
//     delete[] cHeightMap;    // Free the allocated memory for the float height map
//     delete[] cHeightMap8bit; // Free the allocated memory for the 8-bit image
// }

// void midPointDisplacement_(float* fHeightMap, float fRoughness, float fHeight, int iSize, int iLength, glm::ivec2 v2A, glm::ivec2 v2B, glm::ivec2 v2C, glm::ivec2 v2D){
//     // Recursion base case.
//     if (iLength <= 1) return;
//     // Calculate the midpoint
//     glm::ivec2 v2Middle = glm::ivec2(v2A.y + iLength/2, v2A.x + iLength/2);
//     float fAvgHeight = (fHeightMap[v2A.y*iSize + v2A.x] + fHeightMap[v2B.y*iSize + v2B.x] + fHeightMap[v2C.y*iSize + v2C.x] + fHeightMap[v2D.y*iSize + v2D.x]) / 4.0f;
//     fAvgHeight += fHeight + (-fHeight + static_cast<float>(std::rand() % (static_cast<int>(2 * fHeight) + 1)));
//     fHeightMap[v2Middle.y * iSize + v2Middle.x] = fAvgHeight;

//     // Update fHeight
//     float newHeight = fHeight * pow(2, -fRoughness);

//     // Upper left quadrant
//     midPointDisplacement_(fHeightMap, fRoughness, newHeight, iSize, iLength/2, v2A, glm::ivec2(v2A.x + iLength/2, v2A.y), glm::ivec2(v2A.x, v2A.y + iLength/2), v2Middle);
//     // Upper right quadrant
//     midPointDisplacement_(fHeightMap, fRoughness, newHeight, iSize, iLength/2, glm::ivec2(v2A.x + iLength/2, v2A.y), v2B, v2Middle, glm::ivec2(v2B.x, v2B.y+iLength/2));
//     // Lower left quadrant
//     midPointDisplacement_(fHeightMap, fRoughness, newHeight, iSize, iLength/2, glm::ivec2(v2A.x, v2A.y + iLength/2), v2Middle, v2C, glm::ivec2(v2C.x + iLength/2, v2C.y));
//     // Lower right quadrant
//     midPointDisplacement_(fHeightMap, fRoughness, newHeight, iSize, iLength/2, v2Middle, glm::ivec2(v2B.x, v2B.y + iLength/2), glm::ivec2(v2C.x + iLength/2, v2C.y), v2D);
// }

// void CTERRAIN::midPointDisplacement(const int iSize, const char* saveLocation){
//     m_iSize = iSize;
//     float* fHeightMap = new float[m_iSize * m_iSize];
//     std::memset(fHeightMap, 10.0f, m_iSize * m_iSize * sizeof(float));
//     std::srand(static_cast<unsigned int>(time(0)));

//     midPointDisplacement_(fHeightMap, 0.6f, m_iSize, m_iSize, m_iSize, glm::ivec2(0, 0), glm::ivec2(m_iSize-1, 0), glm::ivec2(0, m_iSize-1), glm::ivec2(m_iSize-1, m_iSize-1));

//     // Find the min and max height values in the height map for normalization
//     float minHeight = *std::min_element(fHeightMap, fHeightMap + iSize * iSize);
//     float maxHeight = *std::max_element(fHeightMap, fHeightMap + iSize * iSize);
//     // Allocate an array to hold the 8-bit grayscale data
//     unsigned char* cHeightMap8bit = new unsigned char[iSize * iSize];
//     for(int i = 0; i < m_iSize*m_iSize; i++){
//         cHeightMap8bit[i] = static_cast<unsigned char>(255 * (fHeightMap[i] - minHeight) / (maxHeight - minHeight));
//     }

//     // Store the heightmap as a PNG file
//     stbi_write_png(saveLocation, m_iSize, m_iSize, 1, cHeightMap8bit, m_iSize);

//     // Clean up
//     delete[] fHeightMap;    // Free the allocated memory for the float height map
//     delete[] cHeightMap8bit; // Free the allocated memory for the 8-bit image
// }

// void CTERRAIN::filterHeightBand(float* fpBand, int iStride, int iCount, float fFilter, int iPasses){
//     float v = fpBand[0];
//     int j;

//     // Go through the height band and apply the erosion filter
//     for (int pass = 0; pass < iPasses; pass++){
//         j = iStride;
//         for (int i = 0; i < iCount - 1; i++){
//             fpBand[j] = fFilter * v + (1 - fFilter) * fpBand[j];
//             v = fpBand[j];
            
//             j += iStride;
//         }
//     }
// }