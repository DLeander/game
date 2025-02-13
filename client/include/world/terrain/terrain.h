
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
#include "texture/texture.h"
#include "camera.h"

#include<stb/stb_image_write.h>

#include <algorithm>

struct SHEIGHT_DATA {
            unsigned char* s_pucData;  //the height data
            int s_iSize;              //the height size (power of 2)

            SHEIGHT_DATA() : s_pucData(nullptr), s_iSize(0) {}
};

class CTERRAIN {
    protected:
        

        SHEIGHT_DATA m_heightData; // Height data (Gathered from Heightmap)
        float m_fHeightScale; // Height scale of the terrain

        CSHADER* m_terrainShader;
        unsigned char* m_ucTextureData;
        Texture* m_terrainTexture;

        // Light variables
        int m_iLightDirectionX;
        int m_iLightDirectionZ;
        float m_fMinLightBrightness;
        float m_fMaxLightBrightness;
        float m_fLightSoftness; 

        // Vertex data for the terrain
        std::vector<float> m_vVertices;
        // Color data for the terrain
        std::vector<float> m_vColors;
        // Normalized texture coordinates for the texturing terrain created by m_vVertices.
        std::vector<float> m_vTexCoords;


    public:
        int m_iSize; // Size of the terrain (Must be 2^n)

    // Constructor
    CTERRAIN() : m_terrainShader(nullptr), m_ucTextureData(nullptr), m_terrainTexture(nullptr){}
    // Destructor
    ~CTERRAIN();

    virtual void render( CCAMERA* camera ) = 0;
    virtual void setupBuffers() = 0;
    virtual void createTextureFromHeightMap() = 0;

    // Init the terrain
    void init(const char* filename, int iSize, float fHeightScale);

    // Gather information from heightmap file
    bool loadHeightMap(const char* filename, int iSize);
    bool saveHeightMap(const char* filename);
    bool unloadHeightMap();
    void setColorToTexture(unsigned char* texture, int index, unsigned char R, unsigned char G, unsigned char B);
    void faultFormation(int iMinDelta, int iMaxDelta, int iIterations, const int iSize, const char* saveLocation);
    void midPointDisplacement(const int iSize, const char* saveLocation);
    void filterHeightBand(float* fpBand, int iStride, int iCount, float fFilter, int iPasses);

    // Texture lightning
    inline void setSlopeLightingParams( int iDirX, int iDirZ, float fMinBrightness, float fMaxBrightness, float fSoftness ){ m_iLightDirectionX = iDirX; m_iLightDirectionZ = iDirZ; m_fMinLightBrightness = fMinBrightness;  m_fMaxLightBrightness = fMaxBrightness; m_fLightSoftness = fSoftness; }
    void calculateLightning();

    void setupShader();
    void setupTexture(const char* filename);
    void setupTexture();

    // Set the height scaling variable
    inline void setHeightScale(float fScale) { m_fHeightScale = fScale; }
    // Set the true height value at the given point-ucHeight: the new height value for the point-iX, iZ: which height value to retrieve
    inline void setHeightAtPoint(unsigned char height, int iX, int iZ) { m_heightData.s_pucData[iX + iZ * m_iSize] = height; }
    // A function to get the true height value (0-255) at a point
    inline unsigned char getTrueHeightAtPoint(int iX, int iZ) { return m_heightData.s_pucData[iX + iZ * m_iSize]; }
    // Retrieve the scaled height at a given point
    inline float getScaledHeightAtPoint(int iX, int iZ) { return m_heightData.s_pucData[iX + iZ * m_iSize] * m_fHeightScale; }
    SHEIGHT_DATA getHeightData(){return m_heightData;}
};