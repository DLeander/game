#include "terrainCollision.h"

float CTERRAINCOLLISION::interpolateHeight(CTERRAIN* terrain, float fX, float fZ, float fY) {
    // Ensure coordinates are within valid heightmap range
    int maxX = terrain->m_iSize - 1;
    int maxZ = terrain->m_iSize - 1;
    
    if (fX < 0) fX = 0;
    if (fZ < 0) fZ = 0;
    if (fX > maxX) fX = maxX;
    if (fZ > maxZ) fZ = maxZ;

    int iX1 = floor(fX);
    int iX2 = ceil(fX);
    int iZ1 = floor(fZ);
    int iZ2 = ceil(fZ);

    float fH1 = terrain->getTrueHeightAtPoint(iX1, iZ1);
    float fH2 = terrain->getTrueHeightAtPoint(iX2, iZ1);
    float fH3 = terrain->getTrueHeightAtPoint(iX1, iZ2);
    float fH4 = terrain->getTrueHeightAtPoint(iX2, iZ2);

    float fHa = fH1 + (fH2 - fH1) * (fX - iX1) / (iX2 - iX1);
    float fHb = fH3 + (fH4 - fH3) * (fX - iX1) / (iX2 - iX1);

    float hFinal = fHa + (fHb - fHa) * (fZ - iZ1) / (iZ2 - iZ1);
    return hFinal + 0.00001;
}

float baryCentricInterpolation(glm::vec3 v3P1, glm::vec3 v3P2, glm::vec3 v3P3, glm::vec2 v2Pos){
    float fDet = (v3P2.z - v3P3.z) * (v3P1.x - v3P3.x) + (v3P3.x - v3P2.x) * (v3P1.z - v3P3.z);
    float fL1 = ((v3P2.z - v3P3.z) * (v2Pos.x - v3P3.x) + (v3P3.x - v3P2.x) * (v2Pos.y - v3P3.z)) / fDet;
    float fL2 = ((v3P3.z - v3P1.z) * (v2Pos.x - v3P3.x) + (v3P1.x - v3P3.x) * (v2Pos.y - v3P3.z)) / fDet;
    float fL3 = 1.0f - fL1 - fL2;
    return fL1 * v3P1.y + fL2 * v3P2.y + fL3 * v3P3.y;
}

float CTERRAINCOLLISION::calcCollisionHeightBaryCentric(CTERRAIN* terrain, float iX, float iZ){
    // The size of each grid square (2 triangles make up a grid square)
    float gridSquareSize = terrain->m_iSize / terrain->getHeightData().s_iSize;
    int gridX = floor(iX / gridSquareSize);
    int gridZ = floor(iZ / gridSquareSize);

    float xCoord = fmod(iX, gridSquareSize) / gridSquareSize;
    float zCoord = fmod(iZ, gridSquareSize) / gridSquareSize;

    float answer;
    if (xCoord <= (1 - zCoord)) {
        answer = baryCentricInterpolation(glm::vec3(0, terrain->getTrueHeightAtPoint(gridX, gridZ), 0),
                                          glm::vec3(1, terrain->getTrueHeightAtPoint(gridX + 1, gridZ), 0),
                                          glm::vec3(0, terrain->getTrueHeightAtPoint(gridX, gridZ + 1), 1),
                                          glm::vec2(xCoord, zCoord));
    } else {
        answer = baryCentricInterpolation(glm::vec3(1, terrain->getTrueHeightAtPoint(gridX + 1, gridZ), 0),
                                          glm::vec3(1, terrain->getTrueHeightAtPoint(gridX + 1, gridZ + 1), 1),
                                          glm::vec3(0, terrain->getTrueHeightAtPoint(gridX, gridZ + 1), 1),
                                          glm::vec2(xCoord, zCoord));
    }
    return answer;
}