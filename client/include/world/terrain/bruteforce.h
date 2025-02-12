#pragma once

#include "terrain.h"  // Include the header file for Terrain
// glad
#include<glad/gl.h>
// GLFW
#include<GLFW/glfw3.h>
// GLM
#include<glm/glm.hpp>

class CBRUTEFORCE : public CTERRAIN {
public:
    // Constructor
    CBRUTEFORCE(){}

    // Destructor
    ~CBRUTEFORCE();

    // Implement the Render method from Terrain
    void render(CCAMERA* camera) override;
    void setupBuffers() override;
    void createTextureFromHeightMap() override;

private:
    // Variables
    CVAO* m_terrainVAO;
    CVBO* m_terrainVBO;
    CVBO* m_terrainColorVBO;
    CVBO* m_terrainTexCoordsVBO;
    CEBO* terrainEBO;
};