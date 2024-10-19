#pragma once

#include "terrain.h"  // Include the header file for Terrain
// glad
#include<glad/gl.h>
// GLFW
#include<GLFW/glfw3.h>
// GLM
#include<glm/glm.hpp>

class BruteForce : public Terrain {
public:
    // Constructor
    BruteForce(){}

    // Destructor
    ~BruteForce(){}

    // Implement the Render method from Terrain
    void Render(Camera* camera) override;
};