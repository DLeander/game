
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

struct HEIGHT_DATA {
    unsigned char* pucData; //the height data
    int size;              //the height size (power of 2)
};

class Terrain {
    protected:
        HEIGHT_DATA heightData; // Height data (Gathered from Heightmap)
        float heightScale; // Height scale of the terrain

        Shader* terrainShader;
        VAO* terrainVAO;
        VBO* terrainVBO;
        VBO* terrainColorVBO;
        // EBO* terrainEBO;

        std::vector<float> vertices;
        std::vector<float> colors;

    public:
        int size; // Size of the terrain (Power of 2)

    // Constructor
    Terrain(){}
    // Destructor
    ~Terrain(){unloadHeightMap();}

    virtual void Render( Camera* camera ) = 0;

    // Init the terrain
    void init(const char* filename, int size, float heightScale);

    // Gather information from heightmap file
    bool loadHeightMap(const char* filename, int size);
    bool saveHeightMap(const char* filename);
    bool unloadHeightMap();
    // Generate the vertex data for the terrain and create the VAO, VBO, and EBO.
    void generateVertexData();
    void setupShader();
    void setupBuffers();

    // Set the height scaling variable
    inline void setHeightScale(float scale) { heightScale = scale; }
    // Set the true height value at the given point-ucHeight: the new height value for the point-iX, iZ: which height value to retrieve
    inline void setHeightAtPoint(unsigned char height, int x, int z) { heightData.pucData[x + z * size] = height; }
    // A function to get the true height value (0-255) at a point
    inline unsigned char getTrueHeightAtPoint(int x, int z) { return heightData.pucData[x + z * size]; }
    // Retrieve the scaled height at a given point
    inline float getScaledHeightAtPoint(int x, int z) { return heightData.pucData[x + z * size] * heightScale; }
};