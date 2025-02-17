#pragma once

#include "terrain.h"
#include "geomipmap.h"

class CTERRAINCOLLISION {
public:
    // Constructor: Initialize the collision system with the heightmap
    CTERRAINCOLLISION(){};
    ~CTERRAINCOLLISION(){};
    // Interpolates height values given x and z cord and returns height for the player.
    float interpolateHeight(CTERRAIN* terrain, float iX, float iZ, float iY);

    // Get the terrain height that should be collided with.
    float calcCollisionHeightBaryCentric(CTERRAIN* terrain, float iX, float iZ);
    // Getters
    // Setters
private:
    // Variables
    // Methods
};