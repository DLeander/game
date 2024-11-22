#include "geomipmap.h"

void CGEOMIPMAP::render(CCAMERA* camera) {
    // Activate shader program
    m_terrainShader->Activate();  // Ensure shader is correctly set up
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
}

void CGEOMIPMAP::setupBuffers() {
    // Setup EBOs for each LOD (CALL ONCE)
    // For each patch, go through its vertices and create an index buffer
    for (int z = 0; z < m_iNumPatchesPerSide; z++) {
        for (int x = 0; x < m_iNumPatchesPerSide; x++) {
            // Get the current patch and compute its vertices.
            SGEOMM_PATCH* pCurr = &m_pPatches[z * m_iNumPatchesPerSide + x];
            pCurr->m_vVertices.resize(m_iPatchSize * m_iPatchSize * 3); // Pre-allocate memory
            
            // Compute the vertices
            for (int i = 0; i < m_iPatchSize; i++) {
                for (int j = 0; j < m_iPatchSize; j++) {
                    // Compute the vertices
                    float fX = x * (m_iPatchSize - 1) + j; // Ensure overlap
                    float fZ = z * (m_iPatchSize - 1) + i;
                    float fY = getTrueHeightAtPoint(fX, fZ); // Might need to divide by 255.0f here...
                    
                    // Index to place the vertex in the vector
                    int index = (i * m_iPatchSize + j) * 3; // 3 components per vertex
                    pCurr->m_vVertices[index]     = fX; // X coordinate
                    pCurr->m_vVertices[index + 1] = fY; // Y coordinate
                    pCurr->m_vVertices[index + 2] = fZ; // Z coordinate
                }
            }
            pCurr->m_VAOP = new CVAO();
            pCurr->m_VAOP->Bind();
            pCurr->m_VBOP = new CVBO(pCurr->m_vVertices.data(), pCurr->m_vVertices.size() * sizeof(float));
            pCurr->m_VBOP->Bind();
            // Link the vertices and the texture coordinates to the VAO of the patch.
            pCurr->m_VAOP->LinkAttrib(*pCurr->m_VBOP, 0, 3, GL_FLOAT, 0, (void*)0);
            // pCurr->m_VAOP->LinkAttrib(*m_terrainColorVBO, 1, 3, GL_FLOAT, 0, (void*)0);
            // pCurr->m_VAOP->LinkAttrib(*m_terrainTexCoordsVBO, 2, 3, GL_FLOAT, 0, (void*)0);

            pCurr->m_VAOP->Unbind();
            pCurr->m_VBOP->Unbind();
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
    for (int i = 0; i < m_iPatchSize - 1; i++) {
        for (int j = 0; j < m_iPatchSize - 1; j++) {
            int iTopLeft = i * m_iPatchSize + j;
            int iTopRight = i * m_iPatchSize + (j + 1);
            int iBottomLeft = (i + 1) * m_iPatchSize + j;
            int iBottomRight = (i + 1) * m_iPatchSize + (j + 1);

            m_vIndicesLOD0.push_back(iTopLeft);
            m_vIndicesLOD0.push_back(iBottomLeft);
            m_vIndicesLOD0.push_back(iTopRight);
            m_vIndicesLOD0.push_back(iTopRight);
            m_vIndicesLOD0.push_back(iBottomLeft);
            m_vIndicesLOD0.push_back(iBottomRight);
        }
    }

    // LOD 1 (Half resolution)
    for (int i = 0; i < m_iPatchSize; i += 2) {  // Skip every other row
        for (int j = 0; j < m_iPatchSize; j += 2) {  // Skip every other column
            // Map to full-resolution grid
            int iTopLeft = i * m_iPatchSize + j;
            int iTopRight = i * m_iPatchSize + (j + 2);
            int iBottomLeft = (i + 2) * m_iPatchSize + j;
            int iBottomRight = (i + 2) * m_iPatchSize + (j + 2);

            m_vIndicesLOD1.push_back(iTopLeft);
            m_vIndicesLOD1.push_back(iBottomLeft);
            m_vIndicesLOD1.push_back(iTopRight);

            m_vIndicesLOD1.push_back(iTopRight);
            m_vIndicesLOD1.push_back(iBottomLeft);
            m_vIndicesLOD1.push_back(iBottomRight);
        }
    }

    // LOD 2 (Quarter resolution)
    for (int i = 0; i < m_iPatchSize / 4 - 1; i++) {  // Iterate through a quarter of the vertices
        for (int j = 0; j < m_iPatchSize / 4 - 1; j++) {
            int iTopLeft = (i * 4) * m_iPatchSize + (j * 4);  // Skip every 4th vertex
            int iTopRight = (i * 4) * m_iPatchSize + ((j + 1) * 4);
            int iBottomLeft = ((i + 1) * 4) * m_iPatchSize + (j * 4);
            int iBottomRight = ((i + 1) * 4) * m_iPatchSize + ((j + 1) * 4);

            m_vIndicesLOD2.push_back(iTopLeft);
            m_vIndicesLOD2.push_back(iBottomLeft);
            m_vIndicesLOD2.push_back(iTopRight);
            m_vIndicesLOD2.push_back(iTopRight);
            m_vIndicesLOD2.push_back(iBottomLeft);
            m_vIndicesLOD2.push_back(iBottomRight);
        }
    }

    // LOD 3 (Eighth resolution)
    for (int i = 0; i < m_iPatchSize / 8 - 1; i++) {  // Iterate through an eighth of the vertices
        for (int j = 0; j < m_iPatchSize / 8 - 1; j++) {
            int iTopLeft = (i * 8) * m_iPatchSize + (j * 8);  // Skip every 8th vertex
            int iTopRight = (i * 8) * m_iPatchSize + ((j + 1) * 8);
            int iBottomLeft = ((i + 1) * 8) * m_iPatchSize + (j * 8);
            int iBottomRight = ((i + 1) * 8) * m_iPatchSize + ((j + 1) * 8);

            m_vIndicesLOD3.push_back(iTopLeft);
            m_vIndicesLOD3.push_back(iBottomLeft);
            m_vIndicesLOD3.push_back(iTopRight);
            m_vIndicesLOD3.push_back(iTopRight);
            m_vIndicesLOD3.push_back(iBottomLeft);
            m_vIndicesLOD3.push_back(iBottomRight);

            // Check for out-of-bounds indices
        }
    }
    // Create the EBOs for each LOD
    m_EBOPLOD0 = new CEBO(m_vIndicesLOD0.data(), m_vIndicesLOD0.size() * sizeof(unsigned int));
    m_EBOPLOD1 = new CEBO(m_vIndicesLOD1.data(), m_vIndicesLOD1.size() * sizeof(unsigned int));
    m_EBOPLOD2 = new CEBO(m_vIndicesLOD2.data(), m_vIndicesLOD2.size() * sizeof(unsigned int));
    m_EBOPLOD3 = new CEBO(m_vIndicesLOD3.data(), m_vIndicesLOD3.size() * sizeof(unsigned int));

    std::cout << "Buffers setup" << std::endl;
}

void CGEOMIPMAP::update(CCAMERA* camera){
    for(int z = 0; z < m_iNumPatchesPerSide; z++){
        for(int x = 0; x < m_iNumPatchesPerSide; x++){
            glm::vec3 v3CurrPatch = glm::vec3(x*m_iPatchSize + m_iPatchSize/2, 0, z*m_iPatchSize + m_iPatchSize/2);
            // Compute distance from camera
            float fDistance = glm::distance(camera->m_v3Position, v3CurrPatch);
            // Compute LOD
            int iLOD = 0;
            if(fDistance > 3000){
                iLOD = 2;
            }
            else if(fDistance > 1500){
                iLOD = 2;
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

CGEOMIPMAP::~CGEOMIPMAP(){
    for (int i = 0; i < m_iNumPatchesPerSide * m_iNumPatchesPerSide; i++){
        delete m_pPatches[i].m_VAOP;
        delete m_pPatches[i].m_VBOP;
    }
    delete[] m_pPatches;
    delete m_EBOPLOD0;
    delete m_EBOPLOD1;
    delete m_EBOPLOD2;
    delete m_EBOPLOD3;
    delete m_EBOPDynamic;
}