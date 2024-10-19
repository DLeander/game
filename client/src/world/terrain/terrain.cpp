#include "terrain.h"

void Terrain::init(const char* filename, int size, float heightScale){
    // Set the height scale
    Terrain::heightScale = heightScale;

    // Load the height map
    loadHeightMap(filename, size);

    // Generate vertex and color data
    generateVertexData();

    // Setup the shader
    setupShader();

    // Setup the VAO, VBO
    setupBuffers();
}

bool Terrain::loadHeightMap(const char* filename, int size) {
    FILE* file = fopen(filename, "rb");

    if (heightData.pucData != nullptr) {
        unloadHeightMap();
    }
    Terrain::heightData.pucData = new unsigned char[size * size];
    std::cout << "Heightmap size: " << size << std::endl;
    if (heightData.pucData == nullptr) {
        std::cerr << "Could not allocate memory for height data" << std::endl;
        return false;
    }
    fread(heightData.pucData, 1, size * size, file);

    fclose(file);
    Terrain::size = size;
    Terrain::heightData.size = size;

    std::cout << "Heightmap loaded" << std::endl;
    return true;
}

bool Terrain::unloadHeightMap(){
    if (heightData.pucData == nullptr) {
        delete[] heightData.pucData;
        heightData.pucData = nullptr;
        return true;
    }
    return false;
}

// Generate the vertex data for the terrain
void Terrain::generateVertexData() {
    vertices.clear();
    colors.clear();

    for (int z = 0; z < size - 1; ++z) {
        for (int x = 0; x < size; ++x) {
            // Vertex 1 (z row)
            vertices.push_back(x);
            vertices.push_back(getScaledHeightAtPoint(x, z));
            vertices.push_back(z);
            colors.push_back(getTrueHeightAtPoint(x, z) / 255.0f);
            colors.push_back(getTrueHeightAtPoint(x, z) / 255.0f);
            colors.push_back(getTrueHeightAtPoint(x, z) / 255.0f);

            // Vertex 2 (z+1 row)
            vertices.push_back(x);
            vertices.push_back(getScaledHeightAtPoint(x, z + 1));
            vertices.push_back(z + 1);
            colors.push_back(getTrueHeightAtPoint(x, z + 1) / 255.0f);
            colors.push_back(getTrueHeightAtPoint(x, z + 1) / 255.0f);
            colors.push_back(getTrueHeightAtPoint(x, z + 1) / 255.0f);
        }
    }
}

// Setup the VAO, VBO, and EBO for the terrain
void Terrain::setupBuffers() {
    // Create the VAO, VBO, and EBO
    terrainVAO = new VAO();
    terrainVAO->Bind();

    terrainVBO = new VBO(vertices.data(), vertices.size() * sizeof(float));
    terrainColorVBO = new VBO(colors.data(), colors.size() * sizeof(float));


    terrainVAO->LinkAttrib(*terrainVBO, 0, 3, GL_FLOAT, 0, (void*)0);
    terrainVAO->LinkAttrib(*terrainColorVBO, 1, 3, GL_FLOAT, 0, (void*)0);

    terrainVAO->Unbind();
    terrainVBO->Unbind();
    terrainColorVBO->Unbind();
}

void Terrain::setupShader() {
    terrainShader = new Shader("resources/terrain/terrain.vert", "resources/terrain/terrain.frag");
}