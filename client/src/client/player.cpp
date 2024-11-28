#include "client/player.h"

CPLAYER::CPLAYER(CSHADER* shader) {
    // Initialize the player's position
    m_m4Model = glm::mat4(1.0f);
    m_v3Position = glm::vec3(0.0f, 0.0f, 0.0f);
    m_playerShader = shader;
    init();
    m_bInitialised = true;
}

CPLAYER::CPLAYER(CSHADER* shader, glm::mat4 m4Model){
    m_m4Model = m4Model;
    m_playerShader = shader;
    m_bInitialised = false;
}

CPLAYER::~CPLAYER() {
    // Delete the player's VAO, VBO, and EBO
    if (m_playerVAO) {delete m_playerVAO; m_playerVAO = nullptr;}
    if (m_playerVBO) {delete m_playerVBO; m_playerVBO = nullptr;}
    if (m_playerEBO) {delete m_playerEBO; m_playerEBO = nullptr;}

    // Delete the player's shader
    if (m_playerShader){delete m_playerShader; m_playerShader = nullptr;}
    
    if (m_playerTexture){delete m_playerTexture; m_playerTexture = nullptr;}
}

void CPLAYER::init() {
    m_fSpeed = 0.01f;
    m_v3Position = glm::vec3(0.0f, 0.0f, 0.0f);

    GLfloat vertices[] = {
        // Positions            // Colors           // Texture Coords
        // Front face
        -0.2f, -0.2f,  0.2f,    1.0f, 1.0f, 1.0f,   0.0f, 0.0f,
        0.2f, -0.2f,  0.2f,    1.0f, 1.0f, 1.0f,   1.0f, 0.0f,
        0.2f,  0.2f,  0.2f,    1.0f, 1.0f, 1.0f,   1.0f, 1.0f,
        -0.2f,  0.2f,  0.2f,    1.0f, 1.0f, 1.0f,   0.0f, 1.0f,

        // Back face
        -0.2f, -0.2f, -0.2f,    1.0f, 1.0f, 1.0f,   0.0f, 0.0f,
        0.2f, -0.2f, -0.2f,    1.0f, 1.0f, 1.0f,   1.0f, 0.0f,
        0.2f,  0.2f, -0.2f,    1.0f, 1.0f, 1.0f,   1.0f, 1.0f,
        -0.2f,  0.2f, -0.2f,    1.0f, 1.0f, 1.0f,   0.0f, 1.0f,

        // Left face
        -0.2f, -0.2f, -0.2f,    1.0f, 1.0f, 1.0f,   0.0f, 0.0f,
        -0.2f, -0.2f,  0.2f,    1.0f, 1.0f, 1.0f,   1.0f, 0.0f,
        -0.2f,  0.2f,  0.2f,    1.0f, 1.0f, 1.0f,   1.0f, 1.0f,
        -0.2f,  0.2f, -0.2f,    1.0f, 1.0f, 1.0f,   0.0f, 1.0f,

        // Right face
        0.2f, -0.2f, -0.2f,    1.0f, 1.0f, 1.0f,   0.0f, 0.0f,
        0.2f, -0.2f,  0.2f,    1.0f, 1.0f, 1.0f,   1.0f, 0.0f,
        0.2f,  0.2f,  0.2f,    1.0f, 1.0f, 1.0f,   1.0f, 1.0f,
        0.2f,  0.2f, -0.2f,    1.0f, 1.0f, 1.0f,   0.0f, 1.0f,

        // Top face
        -0.2f,  0.2f, -0.2f,    1.0f, 1.0f, 1.0f,   0.0f, 0.0f,
        -0.2f,  0.2f,  0.2f,    1.0f, 1.0f, 1.0f,   1.0f, 0.0f,
        0.2f,  0.2f,  0.2f,    1.0f, 1.0f, 1.0f,   1.0f, 1.0f,
        0.2f,  0.2f, -0.2f,    1.0f, 1.0f, 1.0f,   0.0f, 1.0f,

        // Bottom face
        -0.2f, -0.2f, -0.2f,    1.0f, 1.0f, 1.0f,   0.0f, 0.0f,
        -0.2f, -0.2f,  0.2f,    1.0f, 1.0f, 1.0f,   1.0f, 0.0f,
        0.2f, -0.2f,  0.2f,    1.0f, 1.0f, 1.0f,   1.0f, 1.0f,
        0.2f, -0.2f, -0.2f,    1.0f, 1.0f, 1.0f,   0.0f, 1.0f
    };

    GLuint indices[] = {
        // Front face
        0, 1, 2,
        2, 3, 0,
        // Back face
        4, 5, 6,
        6, 7, 4,
        // Left face
        8, 9, 10,
        10, 11, 8,
        // Right face
        12, 13, 14,
        14, 15, 12,
        // Top face
        16, 17, 18,
        18, 19, 16,
        // Bottom face
        20, 21, 22,
        22, 23, 20
    };

    m_iIndicesSize = sizeof(indices)/sizeof(int);

    // Init VAO
    m_playerVAO = new CVAO();
    // Bind VAO
	m_playerVAO->Bind();

	// Generates Vertex Buffer Object and links it to vertices
	m_playerVBO = new CVBO(vertices, sizeof(vertices));
	// Generates Element Buffer Object and links it to indices
	m_playerEBO = new CEBO(indices, sizeof(indices));

    // Links VBO attributes such as coordinates and colors to VAO
	m_playerVAO->LinkAttrib(*m_playerVBO, 0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);
	m_playerVAO->LinkAttrib(*m_playerVBO, 1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    m_playerVAO->LinkAttrib(*m_playerVBO, 2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    // Unbind all to prevent accidentally modifying them
    m_playerVAO->Unbind();
    m_playerVBO->Unbind();
    m_playerEBO->Unbind();

    // Load the player's texture
    m_playerTexture = new Texture("resources/textures/player/mulah.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
    m_playerShader->Activate();
    m_playerTexture->textureUnit(m_playerShader, "texture1", 0);
}

// Setup the model matrix for the player
void CPLAYER::setupModelMatrix(CCAMERA* camera) {
    // Calculate the model matrix
    // Calculate the model matrix
    m_m4Model = glm::mat4(1.0f); // Identity matrix for the model

    // Set the model position for the player
    m_m4Model = glm::translate(m_m4Model, m_v3Position); // Translate the model to the player's position

    // Apply rotation to the model based on player's orientation
    float fPlayerRotationAngle = glm::degrees(atan2(m_v3Orientation.z, m_v3Orientation.x));
    m_m4Model = glm::rotate(m_m4Model, glm::radians(-fPlayerRotationAngle), m_v3Up); 

    // Calculate camera position based on player's orientation
    glm::vec3 v3Front = glm::normalize(m_v3Orientation);
    glm::vec3 v3CameraOffset = glm::vec3(0.0f, 0.5f, -2.5f); // Adjust this offset as necessary
    camera->m_v3Position = m_v3Position + v3Front * v3CameraOffset.z + glm::vec3(0.0f, v3CameraOffset.y, 0.0f); // Use the front vector for direction
}

void CPLAYER::setPositionFromModelMatrix() {
    m_v3Position = glm::vec3(m_m4Model[3]);  // Extracts the translation part of the model matrix
}

// General method to draw any player
void CPLAYER::draw(CCAMERA* camera) {
    m_playerShader->Activate();
    // Set the view and projection matrices
    camera->matrix(45.0f, 0.1f, 300.0f, m_playerShader, "camMatrix");
    glUniformMatrix4fv(glGetUniformLocation(m_playerShader->m_ID, "model"), 1, GL_FALSE, glm::value_ptr(m_m4Model));
    // Common rendering code
    m_playerTexture->Bind();
    m_playerVAO->Bind();
    glDrawElements(GL_TRIANGLES, m_iIndicesSize, GL_UNSIGNED_INT, 0);
    m_playerVAO->Unbind();
}

void CPLAYER::drawRemote(){
    m_playerShader->Activate();
    glUniformMatrix4fv(glGetUniformLocation(m_playerShader->m_ID, "model"), 1, GL_FALSE, glm::value_ptr(m_m4Model));
    m_playerTexture->Bind();
    m_playerVAO->Bind();
    glDrawElements(GL_TRIANGLES, m_iIndicesSize, GL_UNSIGNED_INT, 0);
    m_playerVAO->Unbind();
}

void CPLAYER::keyboard_input(GLFWwindow* window) {
	// Handles key inputs
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
		m_v3Position += m_fSpeed * m_v3Orientation;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
		m_v3Position += m_fSpeed * -glm::normalize(glm::cross(m_v3Orientation, m_v3Up));
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
		m_v3Position += m_fSpeed * -m_v3Orientation;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
		m_v3Position += m_fSpeed * glm::normalize(glm::cross(m_v3Orientation, m_v3Up));
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS){
		m_v3Position += m_fSpeed * m_v3Up;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS){
		m_v3Position += m_fSpeed * -m_v3Up;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS){
		m_fSpeed = 0.1f;
	}
	else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE){
		m_fSpeed = 0.01f;
	}
}

void CPLAYER::updateOrientation() {
    // Update the player model's orientation
    glm::vec3 v3Front;
    v3Front.x = cos(glm::radians(m_fYaw));
    v3Front.z = sin(glm::radians(m_fYaw));
    m_v3Orientation = glm::normalize(v3Front);
}

void CPLAYER::mouse_input(GLFWwindow* window, CCAMERA* camera) {
    int iWidth, iHeight;
    glfwGetWindowSize(window, &iWidth, &iHeight);

    // Check if the left mouse button is pressed
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

        double dMouseX, dMouseY;
        glfwGetCursorPos(window, &dMouseX, &dMouseY);

        float fRotX = m_fSensitivity * (float)(dMouseY - (iHeight / 2)); // Vertical movement
        float fRotY = m_fSensitivity * (float)(dMouseX - (iWidth / 2));  // Horizontal movement

        m_fYaw += fRotY;
        m_fPitch += fRotX;

        // Clamp pitch to avoid gimbal lock (prevent looking straight up or down)
        if (m_fPitch > 0) m_fPitch = 0;
        if (m_fPitch < -15.0f) m_fPitch = -15.0f;

        // Update player orientation based on yaw
        updateOrientation();

        // Update camera orientation based on pitch
        camera->updateCameraOrientation(m_fYaw, m_fPitch);

        // Reset cursor position
        glfwSetCursorPos(window, (iWidth / 2), (iHeight / 2));

    } else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
        // Unhide the cursor when LMB is released
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
}

// void CPLAYER::mouse_input(GLFWwindow* window) {
//     int width, height;
//     glfwGetWindowSize(window, &width, &height);

//     // Check if the mouse cursor is disabled
//     if (glfwGetInputMode(window, GLFW_CURSOR) != GLFW_CURSOR_DISABLED) {
//         glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
//         glfwSetCursorPos(window, width / 2, height / 2); // Set to center on first call
//     }

//     // Fetch the current cursor position
//     double mouseX, mouseY;
//     glfwGetCursorPos(window, &mouseX, &mouseY);

//     // Calculate the offset from the center of the screen
//     float rotX = sensitivity * (float)(mouseY - (height / 2)) / height;
//     float rotY = sensitivity * (float)(mouseX - (width / 2)) / width;

//     // Update the yaw and pitch values
//     yaw -= rotY; // Left/right
//     pitch -= rotX; // Up/down

//     // Clamp pitch to prevent flipping at the poles
//     pitch = glm::clamp(pitch, -89.0f, 89.0f);

//     // Update the camera orientation based on yaw and pitch
//     glm::vec3 direction;
//     direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
//     direction.y = sin(glm::radians(pitch));
//     direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
//     orientation = glm::normalize(direction);

//     // Set the cursor position back to the center
//     glfwSetCursorPos(window, width / 2, height / 2);
// }
