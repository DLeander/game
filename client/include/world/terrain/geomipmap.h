#pragma once

#include "terrain.h"  // Include the header file for Terrain

// glad
#include<glad/gl.h>
// GLFW
#include<GLFW/glfw3.h>
// GLM
#include<glm/glm.hpp>

#include <string>

class CGEOMIPMAP : public CTERRAIN {
public:

    struct SGEOMM_PATCH {
        float m_fDistance; // Distance from the camera
        int m_iLOD;       // Level of detail
        std::vector<float> m_vVertices; // Vertices
        std::vector<float> m_vTexCoords; // Texture coordinates
        CVBO* m_VBOP;       // Vertex Buffer Object
        CVBO* m_VBOPT;    // Vertex Buffer Object for texture coordinates
        CVAO* m_VAOP;    // Vertex Array Object

        SGEOMM_PATCH() : m_fDistance(0.0f), m_iLOD(0), m_VBOP(nullptr), m_VBOPT(nullptr), m_VAOP(nullptr) {}
    };

    struct SGEOMM_GRID {
        std::vector<SGEOMM_PATCH*> s_vPatches;
        std::string s_sGridHeightMapPath;
        SHEIGHT_DATA s_HeightData;
        glm::vec2 s_v2GridTopLeftCoord;
        glm::vec2 s_v2GridBottomRightCoord;
    };

    // Constructor
    CGEOMIPMAP(int iSize, int iNumPatchesPerSide) : m_iNumPatchesPerSide(iNumPatchesPerSide) { m_iSize = iSize; m_pPatches = new SGEOMM_PATCH[iNumPatchesPerSide * iNumPatchesPerSide];}
    CGEOMIPMAP() { m_iSize = 4096; m_iPatchSize = 64; m_iGridSize = m_iPatchSize*4; m_iNumGridPerSide = m_iSize / (m_iPatchSize * 2); m_Grids = new SGEOMM_GRID[m_iNumGridPerSide * m_iNumGridPerSide]; m_iNumPatchesPerSide = (m_iSize)/(m_iPatchSize); m_pPatches = new SGEOMM_PATCH[m_iNumPatchesPerSide * m_iNumPatchesPerSide];}
    // CGEOMIPMAP(){ m_iSize = 64; m_iPatchSize = 64; m_iGridSize = m_iPatchSize*4; m_iNumGridPerSide = m_iSize/m_iGridSize; m_Grids = new SGEOMM_GRID[m_iNumGridPerSide * m_iNumGridPerSide]; m_iNumPatchesPerSide = (m_iSize)/(m_iPatchSize); m_pPatches = new SGEOMM_PATCH[m_iNumPatchesPerSide * m_iNumPatchesPerSide];}
    // Destructor
    ~CGEOMIPMAP();

    // Implement the Render method from Terrain
    void render(CCAMERA* camera) override;
    void setupBuffers() override;
    void createTextureFromHeightMap() override;
    void setupGrids() override;
    void update(CCAMERA* camera);

private:
    // Variables
    SGEOMM_PATCH* m_pPatches;
    SGEOMM_GRID* m_Grids;
    int m_iNumPatchesPerSide;
    int m_iPatchSize;
    int m_iGridSize;
    int m_iNumGridPerSide;
    std::vector<unsigned int> m_vIndicesLOD0; // Indices for LOD 0
    std::vector<unsigned int> m_vIndicesLOD1; // Indices for LOD 1
    std::vector<unsigned int> m_vIndicesLOD2; // Indices for LOD 2
    CEBO* m_EBOPLOD0;          // Element Buffer Object for LOD 0
    CEBO* m_EBOPLOD1;         // Element Buffer Object for LOD 1
    CEBO* m_EBOPLOD2;        // Element Buffer Object for LOD 2

    // The grids to render given player position.
    std::vector<SGEOMM_GRID*> m_vGridsToRender;
};