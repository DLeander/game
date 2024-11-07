
#pragma once

#include <fstream>
#include <iostream>
#include <vector>

// glad
#include<glad/gl.h>
// GLFW
#include<GLFW/glfw3.h>
// GLM
#include<glm/glm.hpp>

#include "shader/shaderClass.h"
#include "objects/VAO.h"
#include "objects/VBO.h"
#include "objects/EBO.h"
#include "camera.h"

#include<stb/stb_image_write.h>

#include <algorithm>

struct SHEIGHT_DATA {
    unsigned char* s_pucData; //the height data
    int s_iSize;              //the height size (power of 2)
};

class CTERRAIN {
    protected:
        SHEIGHT_DATA m_heightData; // Height data (Gathered from Heightmap)
        float m_fHeightScale; // Height scale of the terrain

        CSHADER* m_terrainShader;
        CVAO* m_terrainVAO;
        CVBO* m_terrainVBO;
        CVBO* m_terrainColorVBO;
        // EBO* terrainEBO;

        std::vector<float> m_vVertices;
        std::vector<float> m_vColors;

    public:
        int m_iSize; // Size of the terrain (Power of 2)

    // Constructor
    CTERRAIN(){}
    // Destructor
    ~CTERRAIN(){unloadHeightMap();}

    virtual void render( CCAMERA* camera ) = 0;

    // Init the terrain
    void init(const char* filename, int iSize, float fHeightScale);

    // Gather information from heightmap file
    bool loadHeightMap(const char* filename, int iSize);
    bool saveHeightMap(const char* filename);
    bool unloadHeightMap();
    void faultFormation(int iMinDelta, int iMaxDelta, int iIterations, const int iSize, const char* saveLocation);
    void midPointDisplacement(const int iSize, const char* saveLocation);
    void filterHeightBand(float* fpBand, int iStride, int iCount, float fFilter, int iPasses);

    // Generate the vertex data for the terrain and create the VAO, VBO, and EBO.
    void generateVertexData();
    void setupShader();
    void setupBuffers();

    // Set the height scaling variable
    inline void setHeightScale(float fScale) { m_fHeightScale = fScale; }
    // Set the true height value at the given point-ucHeight: the new height value for the point-iX, iZ: which height value to retrieve
    inline void setHeightAtPoint(unsigned char height, int iX, int iZ) { m_heightData.s_pucData[iX + iZ * m_iSize] = height; }
    // A function to get the true height value (0-255) at a point
    inline unsigned char getTrueHeightAtPoint(int iX, int iZ) { return m_heightData.s_pucData[iX + iZ * m_iSize]; }
    // Retrieve the scaled height at a given point
    inline float getScaledHeightAtPoint(int iX, int iZ) { return m_heightData.s_pucData[iX + iZ * m_iSize] * m_fHeightScale; }
};