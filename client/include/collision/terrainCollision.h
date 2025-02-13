#pragma once

#include "terrain.h"
#include "geomipmap.h"

class CTERRAINCOLLISION {
public:
    // Constructor: Initialize the collision system with the heightmap
    CTERRAINCOLLISION(){};
    ~CTERRAINCOLLISION(){};
    // Interpolates height values given x and z cord and returns height for the player.
    float interpolateHeight(float iX, float iZ, float iY);

    // Getters
    SHEIGHT_DATA getHeightData(){ return m_heightData; }
    // Setters
    void setHeightData(const SHEIGHT_DATA& heightData){ m_heightData.s_pucData = heightData.s_pucData;m_heightData.s_iSize = heightData.s_iSize; }
private:
    SHEIGHT_DATA m_heightData; // Height data (Gathered from Heightmap)
    // Same method can be found in terrain.h: fix.
    unsigned char getTrueHeightAtPoint(int iX, int iZ) { return m_heightData.s_pucData[iX + iZ * m_heightData.s_iSize]; }
};