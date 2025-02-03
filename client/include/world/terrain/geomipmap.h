#pragma once

#include "terrain.h"  // Include the header file for Terrain

// glad
#include<glad/gl.h>
// GLFW
#include<GLFW/glfw3.h>
// GLM
#include<glm/glm.hpp>

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

        SGEOMM_PATCH() : m_fDistance(0.0f), m_iLOD(0) {}
    };
    // Constructor
    CGEOMIPMAP(int iSize, int iNumPatchesPerSide) : m_iNumPatchesPerSide(iNumPatchesPerSide) { m_iSize = iSize; m_pPatches = new SGEOMM_PATCH[iNumPatchesPerSide * iNumPatchesPerSide];}
    CGEOMIPMAP(){ m_iSize = 512; m_iPatchSize = 32; m_iNumPatchesPerSide = m_iSize/m_iPatchSize; m_pPatches = new SGEOMM_PATCH[m_iNumPatchesPerSide * m_iNumPatchesPerSide];}

    // Destructor
    ~CGEOMIPMAP();

    // Variables
    std::vector<unsigned int> m_vIndicesLOD0; // Indices for LOD 0
    std::vector<unsigned int> m_vIndicesLOD1; // Indices for LOD 1
    std::vector<unsigned int> m_vIndicesLOD2; // Indices for LOD 2
    std::vector<unsigned int> m_vIndicesLOD3; // Indices for LOD 3
    CEBO* m_EBOPLOD0;          // Element Buffer Object for LOD 0
    CEBO* m_EBOPLOD1;         // Element Buffer Object for LOD 1
    CEBO* m_EBOPLOD2;        // Element Buffer Object for LOD 2
    CEBO* m_EBOPLOD3;       // Element Buffer Object for LOD 3

    // Implement the Render method from Terrain
    void render(CCAMERA* camera) override;
    void setupBuffers() override;
    void createTextureFromHeightMap() override;
    void update(CCAMERA* camera);
    // void setupBuffers();

private:
    SGEOMM_PATCH* m_pPatches;
    int m_iNumPatchesPerSide;
    int m_iPatchSize;
    glm::vec3 prevPatch;
};