#include "geomipmap.h"

void CGEOMIPMAP::render(CCAMERA* camera) {
    // Activate shader program
    m_terrainShader->Activate();
    camera->matrix(45.0f, 0.1f, 750.0f, m_terrainShader, "camMatrix");
    GLint location = glGetUniformLocation(m_terrainShader->m_ID, "camMatrix");
    if (location == -1) {
        std::cerr << "Uniform 'camMatrix' not found in shader." << std::endl;
    }

    update(camera);
    m_terrainTexture->Bind();
    for (int patch = 0; patch < m_iNumPatchesPerSide * m_iNumPatchesPerSide; patch++){
        SGEOMM_PATCH* pCurr = &m_pPatches[patch];
        // Set the model matrix
        // Bind the VAO that stores vertex and color data
        pCurr->m_VAOP->Bind();
        int count;
        switch (pCurr->m_iLOD) {
            case 0:
                m_EBOPLOD0->Bind();
                count = m_vIndicesLOD0.size();
                break;
            case 1:
                m_EBOPLOD1->Bind();
                count = m_vIndicesLOD1.size();
                break;
            case 2:
                m_EBOPLOD2->Bind();
                count = m_vIndicesLOD2.size();
                break;
            case 3:
                m_EBOPLOD3->Bind();
                count = m_vIndicesLOD3.size();
                break;
            default:
                m_EBOPLOD0->Bind();
                count = m_vIndicesLOD0.size();
                break;
        }
        // Draw the patch
        glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, 0);
        // Unbind the VAO
        pCurr->m_VAOP->Unbind();
        // Unbind EBO
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // Unbind EBO
    }
    m_terrainShader->DeActivate();
    m_terrainTexture->Unbind();
}

void CGEOMIPMAP::update(CCAMERA* camera){
    for(int z = 0; z < m_iNumPatchesPerSide; z++){
        for(int x = 0; x < m_iNumPatchesPerSide; x++){
            glm::vec3 v3CurrPatch = glm::vec3(x*m_iPatchSize + m_iPatchSize/2, 0, z*m_iPatchSize + m_iPatchSize/2);
            // Compute distance from camera
            float fDistance = glm::distance(camera->m_v3Position, v3CurrPatch);
            // Compute LOD
            int iLOD = 0;
            if (fDistance > 700){
                iLOD = 1;
            }
            else if(fDistance > 500){
                iLOD = 1;
            }
            else if(fDistance > 300){
                iLOD = 1;
            }
            else{
                iLOD = 0;
            }
            // Update patch
            m_pPatches[z*m_iNumPatchesPerSide + x].m_fDistance = fDistance;
            m_pPatches[z*m_iNumPatchesPerSide + x].m_iLOD = iLOD;
        }
    }
}

void CGEOMIPMAP::setupBuffers() {
    // Setup EBOs for each LOD (CALL ONCE)
    // This local vector stores skirt vertices which are then added to the end of vVertices so that it will be easier to NOT generate them for certain LODS.
    std::vector<float> vSkirtVerticesTop;
    std::vector<float> vSkirtVerticesLeft;
    std::vector<float> vSkirtVerticesRight;
    std::vector<float> vSkirtVerticesBottom;
    int iSkirtHeight = 2.0f;
    // For each patch, go through its vertices and create an index buffer
    for (int z = 0; z < m_iNumPatchesPerSide; z++) {
        for (int x = 0; x < m_iNumPatchesPerSide; x++) {
            vSkirtVerticesTop.clear();
            vSkirtVerticesLeft.clear();
            vSkirtVerticesRight.clear();
            vSkirtVerticesBottom.clear();

            // Get the current patch and compute its vertices.
            SGEOMM_PATCH* pCurr = &m_pPatches[z * m_iNumPatchesPerSide + x];
            pCurr->m_vVertices.clear();
            pCurr->m_vTexCoords.clear();
            pCurr->m_vVertices.resize(m_iPatchSize * m_iPatchSize * 3); // Pre-allocate memory for terrain vertices
            pCurr->m_vTexCoords.resize(m_iPatchSize * m_iPatchSize * 2); // Pre-allocate memory texture coordinates

            // Compute the vertices
            for (int i = 0; i < m_iPatchSize; i++) {
                for (int j = 0; j < m_iPatchSize; j++) {
                    // Compute the vertices
                    float fX = x * (m_iPatchSize - 1) + j;
                    float fZ = z * (m_iPatchSize - 1) + i;
                    float fY = getTrueHeightAtPoint(fX, fZ);
                    
                    // Index to place the vertex in the vector
                    int iVIndex = (i * m_iPatchSize + j) * 3; // 3 components per vertex
                    pCurr->m_vVertices[iVIndex] = fX; // X coordinate
                    pCurr->m_vVertices[iVIndex+1] = fY; // Y coordinate
                    pCurr->m_vVertices[iVIndex+2] = fZ; // Z coordinate

                    int iCIndex = (i * m_iPatchSize + j) * 2;
                    pCurr->m_vTexCoords[iCIndex] = (static_cast<float>(fX) / (m_iSize));
                    pCurr->m_vTexCoords[iCIndex+1] = (static_cast<float>(fZ) / (m_iSize));

                    if (i == 0){
                        vSkirtVerticesTop.push_back(fX);
                        vSkirtVerticesTop.push_back(fY - iSkirtHeight); // Lowered height
                        vSkirtVerticesTop.push_back(fZ);
                    }
                    if (j == 0){
                        vSkirtVerticesLeft.push_back(fX);
                        vSkirtVerticesLeft.push_back(fY - iSkirtHeight); // Lowered height
                        vSkirtVerticesLeft.push_back(fZ);
                    }
                    if (j == m_iPatchSize - 1){
                        vSkirtVerticesRight.push_back(fX);
                        vSkirtVerticesRight.push_back(fY - iSkirtHeight); // Lowered height
                        vSkirtVerticesRight.push_back(fZ);
                    }
                    if (i == m_iPatchSize - 1){
                        vSkirtVerticesBottom.push_back(fX);
                        vSkirtVerticesBottom.push_back(fY - iSkirtHeight); // Lowered height
                        vSkirtVerticesBottom.push_back(fZ);
                    }
                }
            }
            // Add the skirts to the end of the vertices vector. ORDER IS: TOP LEFT RIGHT BOTTOM (Should be 32 of each)
            pCurr->m_vVertices.insert(pCurr->m_vVertices.end(), vSkirtVerticesTop.begin(), vSkirtVerticesTop.end());
            pCurr->m_vVertices.insert(pCurr->m_vVertices.end(), vSkirtVerticesLeft.begin(), vSkirtVerticesLeft.end());
            pCurr->m_vVertices.insert(pCurr->m_vVertices.end(), vSkirtVerticesRight.begin(), vSkirtVerticesRight.end());
            pCurr->m_vVertices.insert(pCurr->m_vVertices.end(), vSkirtVerticesBottom.begin(), vSkirtVerticesBottom.end());

            pCurr->m_VAOP = new CVAO();
            pCurr->m_VAOP->Bind();
            pCurr->m_VBOP = new CVBO(pCurr->m_vVertices.data(), pCurr->m_vVertices.size() * sizeof(float));
            pCurr->m_VBOP->Bind();
            pCurr->m_VBOPT = new CVBO(pCurr->m_vTexCoords.data(), pCurr->m_vTexCoords.size() * sizeof(float));
            pCurr->m_VBOPT->Bind();
            // Link the vertices and the texture coordinates to the VAO of the patch.
            pCurr->m_VAOP->LinkAttrib(*pCurr->m_VBOP, 0, 3, GL_FLOAT, 0, (void*)0);
            pCurr->m_VAOP->LinkAttrib(*pCurr->m_VBOPT, 2, 2, GL_FLOAT, 0, (void*)0);

            pCurr->m_VAOP->Unbind();
            pCurr->m_VBOP->Unbind();
            pCurr->m_VBOPT->Unbind();
        }
    }
    
    // Create the different indices for each LOD.
    // *--------*
    // |      / |
    // |    /   |
    // |  /     |
    // *--------*

    // We loop from 0 to m_iPatchSize-1 as we always need to have a triangle setup.
    // LOD 0 (Full resolution)
    for (int i = 0; i < m_iPatchSize; i++) {
        for (int j = 0; j < m_iPatchSize; j++) {
            int iTopLeft = i * (m_iPatchSize) + j;
            int iTopRight = i * (m_iPatchSize) + std::min(j + 1, m_iPatchSize - 1);
            int iBottomLeft = std::min(i + 1, m_iPatchSize - 1) * (m_iPatchSize) + j;
            int iBottomRight = std::min(i + 1, m_iPatchSize - 1) * (m_iPatchSize) + std::min(j + 1, m_iPatchSize - 1);

            m_vIndicesLOD0.push_back(iTopLeft);
            m_vIndicesLOD0.push_back(iBottomLeft);
            m_vIndicesLOD0.push_back(iTopRight);
            m_vIndicesLOD0.push_back(iTopRight);
            m_vIndicesLOD0.push_back(iBottomLeft);
            m_vIndicesLOD0.push_back(iBottomRight);
        }
    }

    // LOD 1 (Half resolution)
    // Skirts start at the end of the vertices vector.
    int iSkirtIndexTop = m_iPatchSize * m_iPatchSize;
    int iSkirtIndexLeft = m_iPatchSize * m_iPatchSize + m_iPatchSize;
    int iSkirtIndexRight = m_iPatchSize * m_iPatchSize + 2*(m_iPatchSize);
    int iSkirtIndexBottom = m_iPatchSize * m_iPatchSize + 3*(m_iPatchSize);
    for (int i = 0; i < m_iPatchSize; i += 2) {
        for (int j = 0; j < m_iPatchSize; j += 2) {

            int iTopLeft = i * m_iPatchSize + j;

            int iTopRight = i * m_iPatchSize + std::min(j + 2, m_iPatchSize - 1); // Prevent out-of-bounds

            int iBottomLeft = std::min(i + 2, m_iPatchSize - 1) * m_iPatchSize + j;

            int iBottomRight = std::min(i + 2, m_iPatchSize - 1) * m_iPatchSize + std::min(j + 2, m_iPatchSize - 1);

            // Avoid pushing indices out of bounds
            if (iTopLeft >= m_iPatchSize * m_iPatchSize || 
                iTopRight >= m_iPatchSize * m_iPatchSize || 
                iBottomLeft >= m_iPatchSize * m_iPatchSize || 
                iBottomRight >= m_iPatchSize * m_iPatchSize) {
                std::cout << "Out of bounds: i=" << i << " j=" << j << std::endl;
                continue;
            }

            // Push the triangle indices
            m_vIndicesLOD1.push_back(iTopLeft);
            m_vIndicesLOD1.push_back(iBottomLeft);
            m_vIndicesLOD1.push_back(iTopRight);

            m_vIndicesLOD1.push_back(iTopRight);
            m_vIndicesLOD1.push_back(iBottomLeft);
            m_vIndicesLOD1.push_back(iBottomRight);

            // If we are at the edge of LOD1 patch, then we add a skirt to fix cracks between different LODs.
            // **Skirt Indices for LOD Cracks**
            if (i == 0 && iSkirtIndexTop < m_iPatchSize*m_iPatchSize + m_iPatchSize - 1) {  // Top Skirt
                m_vIndicesLOD1.push_back(iTopLeft);
                m_vIndicesLOD1.push_back(iSkirtIndexTop);
                m_vIndicesLOD1.push_back(iTopRight);

                m_vIndicesLOD1.push_back(iTopRight);
                m_vIndicesLOD1.push_back(iSkirtIndexTop);
                m_vIndicesLOD1.push_back(iSkirtIndexTop + 2);

                iSkirtIndexTop += 2;
            }

            if (j == 0 && iSkirtIndexLeft <  m_iPatchSize*m_iPatchSize + 2*(m_iPatchSize) - 1) { // Left Skirt
                m_vIndicesLOD1.push_back(iSkirtIndexLeft);
                m_vIndicesLOD1.push_back(iSkirtIndexLeft + 2);
                m_vIndicesLOD1.push_back(iTopLeft);

                m_vIndicesLOD1.push_back(iTopLeft);
                m_vIndicesLOD1.push_back(iSkirtIndexLeft + 2);
                m_vIndicesLOD1.push_back(iBottomLeft);

                iSkirtIndexLeft += 2;
            }

            if (j == m_iPatchSize - 1 && iSkirtIndexRight < m_iPatchSize*m_iPatchSize + 3*(m_iPatchSize) - 1) { // Right Skirt
                m_vIndicesLOD1.push_back(iTopRight);
                m_vIndicesLOD1.push_back(iSkirtIndexRight);
                m_vIndicesLOD1.push_back(iBottomRight);

                m_vIndicesLOD1.push_back(iSkirtIndexRight);
                m_vIndicesLOD1.push_back(iBottomRight);
                m_vIndicesLOD1.push_back(iSkirtIndexRight+2);

                iSkirtIndexRight += 2;
            }

            if (i == m_iPatchSize - 1 && iSkirtIndexBottom < m_iPatchSize*m_iPatchSize + 4*(m_iPatchSize) - 1) { // Bottom Skirt
                m_vIndicesLOD1.push_back(iBottomLeft);  // Top-left of the patch
                m_vIndicesLOD1.push_back(iBottomRight); // Top-right of the patch
                m_vIndicesLOD1.push_back(iSkirtIndexBottom);  // Bottom-left (skirt vertex)

                m_vIndicesLOD1.push_back(iSkirtIndexBottom);   // Bottom-left (skirt vertex)
                m_vIndicesLOD1.push_back(iBottomRight);  // Top-right of the patch
                m_vIndicesLOD1.push_back(iSkirtIndexBottom + 2); // Bottom-right (skirt vertex)

                iSkirtIndexBottom += 2;
            }
        }
    }

    // LOD 2 (Quarter resolution)
    for (int i = 0; i < m_iPatchSize; i += 4) {
        for (int j = 0; j < m_iPatchSize; j += 4) {
            int iTopLeft = i * m_iPatchSize + j;
            int iTopRight = i * m_iPatchSize + std::min(j + 4, m_iPatchSize - 1); // Prevent out-of-bounds
            int iBottomLeft = std::min(i + 4, m_iPatchSize - 1) * m_iPatchSize + j;
            int iBottomRight = std::min(i + 4, m_iPatchSize - 1) * m_iPatchSize + std::min(j + 4, m_iPatchSize - 1);

            // Avoid pushing indices out of bounds
            if (iTopLeft >= m_iPatchSize * m_iPatchSize || 
                iTopRight >= m_iPatchSize * m_iPatchSize || 
                iBottomLeft >= m_iPatchSize * m_iPatchSize || 
                iBottomRight >= m_iPatchSize * m_iPatchSize) {
                std::cout << "Out of bounds: i=" << i << " j=" << j << std::endl;
                continue;
            }

            // Push the triangle indices
            m_vIndicesLOD2.push_back(iTopLeft);
            m_vIndicesLOD2.push_back(iBottomLeft);
            m_vIndicesLOD2.push_back(iTopRight);

            m_vIndicesLOD2.push_back(iTopRight);
            m_vIndicesLOD2.push_back(iBottomLeft);
            m_vIndicesLOD2.push_back(iBottomRight);
        }
    }

    // LOD 3 (Eighth resolution)
    for (int i = 0; i < m_iPatchSize; i += 8) {
        for (int j = 0; j < m_iPatchSize; j += 8) {
            int iTopLeft = i * m_iPatchSize + j;
            int iTopRight = i * m_iPatchSize + std::min(j + 8, m_iPatchSize - 1); // Prevent out-of-bounds
            int iBottomLeft = std::min(i + 8, m_iPatchSize - 1) * m_iPatchSize + j;
            int iBottomRight = std::min(i + 8, m_iPatchSize - 1) * m_iPatchSize + std::min(j + 8, m_iPatchSize - 1);

            // Avoid pushing indices out of bounds
            if (iTopLeft >= m_iPatchSize * m_iPatchSize || 
                iTopRight >= m_iPatchSize * m_iPatchSize || 
                iBottomLeft >= m_iPatchSize * m_iPatchSize || 
                iBottomRight >= m_iPatchSize * m_iPatchSize) {
                std::cout << "Out of bounds: i=" << i << " j=" << j << std::endl;
                continue;
            }

            // Push the triangle indices
            m_vIndicesLOD3.push_back(iTopLeft);
            m_vIndicesLOD3.push_back(iBottomLeft);
            m_vIndicesLOD3.push_back(iTopRight);

            m_vIndicesLOD3.push_back(iTopRight);
            m_vIndicesLOD3.push_back(iBottomLeft);
            m_vIndicesLOD3.push_back(iBottomRight);
        }
    }
    // Create the EBOs for each LOD
    m_EBOPLOD0 = new CEBO(m_vIndicesLOD0.data(), m_vIndicesLOD0.size() * sizeof(unsigned int));
    m_EBOPLOD1 = new CEBO(m_vIndicesLOD1.data(), m_vIndicesLOD1.size() * sizeof(unsigned int));
    m_EBOPLOD2 = new CEBO(m_vIndicesLOD2.data(), m_vIndicesLOD2.size() * sizeof(unsigned int));
    m_EBOPLOD3 = new CEBO(m_vIndicesLOD3.data(), m_vIndicesLOD3.size() * sizeof(unsigned int));

    std::cout << "Buffers setup" << std::endl;
}

void CGEOMIPMAP::createTextureFromHeightMap() {
    // Define RGB colors for green (low altitude) and gray (high altitude)
    unsigned char green[3] = {65, 155, 55}; // Grass color
    unsigned char gray[3] = {81, 81, 81};   // Rock color

    // Allocate memory for the color map
    unsigned char* ucColorMap = new unsigned char[m_iSize * m_iSize * 3];

    for (int i = 0; i < m_iSize * m_iSize; i++) {
        // Normalize the height value (0 to 1 range)
        float fHeight = static_cast<float>(m_heightData.s_pucData[i]) / 255.0f;
        
        // Interpolate between colors based on height (low = green, high = gray)
        unsigned char ucR = static_cast<unsigned char>(fHeight * gray[0] + (1.0f - fHeight)/2 * green[0]);
        unsigned char ucG = static_cast<unsigned char>(fHeight * gray[1] + (1.0f - fHeight)/2 * green[1]);
        unsigned char ucB = static_cast<unsigned char>(fHeight * gray[2] + (1.0f - fHeight)/2 * green[2]);
        if (i*3 < m_iSize * m_iSize * 3){
            setColorToTexture(ucColorMap, i*3, ucR, ucG, ucB);
        }
    }

    m_ucTextureData = ucColorMap;
}

CGEOMIPMAP::~CGEOMIPMAP(){
    if (m_pPatches){
        for (int i = 0; i < m_iNumPatchesPerSide * m_iNumPatchesPerSide; i++){
            if (m_pPatches[i].m_VAOP) {
                m_pPatches[i].m_VAOP->Delete();
                delete m_pPatches[i].m_VAOP;
                m_pPatches[i].m_VAOP = nullptr;
            }
            if (m_pPatches[i].m_VBOP) {
                m_pPatches[i].m_VBOP->Delete();
                delete m_pPatches[i].m_VBOP;
                m_pPatches[i].m_VBOP = nullptr;
            }
            if (m_pPatches[i].m_VBOPT) {
                m_pPatches[i].m_VBOPT->Delete();
                delete m_pPatches[i].m_VBOPT;
                m_pPatches[i].m_VBOPT = nullptr;
            }
        }
        delete[] m_pPatches;
        m_pPatches = nullptr;
    }
    
    if (m_EBOPLOD0){m_EBOPLOD0->Delete(); delete m_EBOPLOD0; m_EBOPLOD0 = nullptr;}
    if (m_EBOPLOD1){m_EBOPLOD1->Delete(); delete m_EBOPLOD1; m_EBOPLOD1 = nullptr;}
    if (m_EBOPLOD2){m_EBOPLOD2->Delete(); delete m_EBOPLOD2; m_EBOPLOD2 = nullptr;}
    if (m_EBOPLOD3){m_EBOPLOD3->Delete(); delete m_EBOPLOD3; m_EBOPLOD3 = nullptr;}
}