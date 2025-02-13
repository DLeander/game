#include "terrainCollision.h"

float CTERRAINCOLLISION::interpolateHeight(float fX, float fZ, float fY) {
    // Ensure coordinates are within valid heightmap range
    int maxX = m_heightData.s_iSize - 1;
    int maxZ = m_heightData.s_iSize - 1;
    
    if (fX < 0) fX = 0;
    if (fZ < 0) fZ = 0;
    if (fX > maxX) fX = maxX;
    if (fZ > maxZ) fZ = maxZ;

    int iX1 = floor(fX);
    int iX2 = ceil(fX);
    int iZ1 = floor(fZ);
    int iZ2 = ceil(fZ);

    float fH1 = getTrueHeightAtPoint(iX1, iZ1);
    float fH2 = getTrueHeightAtPoint(iX2, iZ1);
    float fH3 = getTrueHeightAtPoint(iX1, iZ2);
    float fH4 = getTrueHeightAtPoint(iX2, iZ2);

    float fHa = fH1 + (fH2 - fH1) * (fX - iX1) / (iX2 - iX1);
    float fHb = fH3 + (fH4 - fH3) * (fX - iX1) / (iX2 - iX1);

    float hFinal = fHa + (fHb - fHa) * (fZ - iZ1) / (iZ2 - iZ1);
    return hFinal + 0.00001;
}
