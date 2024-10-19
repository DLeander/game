#include "client/player.h"

Player::Player(Shader* shader) {
    // Initialize the player's position
    model = glm::mat4(1.0f);
    position = glm::vec3(0.0f, 0.0f, 0.0f);
    playerShader = shader;
    init();
    initialised = true;
}

Player::Player(Shader* shader, glm::mat4 model){
    model = model;
    playerShader = shader;
    initialised = false;
}

Player::~Player() {
    // Delete the player's VAO, VBO, and EBO
    playerVAO->Delete();
    playerVBO->Delete();
    playerEBO->Delete();
    delete playerVAO;
    delete playerVBO;
    delete playerEBO;

    // Delete the player's shader
    playerShader->Delete();
    delete playerShader;

    playerTexture->Delete();
}

void Player::init() {
    speed = 0.01f;
    position = glm::vec3(0.0f, 0.0f, 0.0f);
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
    indices_size = sizeof(indices)/sizeof(int);

    // Init VAO
    playerVAO = new VAO();
    // Bind VAO
	playerVAO->Bind();

	// Generates Vertex Buffer Object and links it to vertices
	playerVBO = new VBO(vertices, sizeof(vertices));
	// Generates Element Buffer Object and links it to indices
	playerEBO = new EBO(indices, sizeof(indices));

    // Links VBO attributes such as coordinates and colors to VAO
	playerVAO->LinkAttrib(*playerVBO, 0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);
	playerVAO->LinkAttrib(*playerVBO, 1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    playerVAO->LinkAttrib(*playerVBO, 2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    // Unbind all to prevent accidentally modifying them
    playerVAO->Unbind();
    playerVBO->Unbind();
    playerEBO->Unbind();

    // Load the player's texture
    playerTexture = new Texture("resources/textures/player/mulah.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
    playerShader->Activate();
    playerTexture->textureUnit(playerShader, "texture1", 0);
}

// Setup the model matrix for the player
void Player::setupModelMatrix(Camera* camera) {
    // Calculate the model matrix
    // Calculate the model matrix
    model = glm::mat4(1.0f); // Identity matrix for the model

    // Set the model position for the player
    model = glm::translate(model, position); // Translate the model to the player's position

    // Apply rotation to the model based on player's orientation
    float playerRotationAngle = glm::degrees(atan2(orientation.z, orientation.x));
    model = glm::rotate(model, glm::radians(-playerRotationAngle), up); 

    // Calculate camera position based on player's orientation
    glm::vec3 front = glm::normalize(orientation);
    glm::vec3 cameraOffset = glm::vec3(0.0f, 0.5f, -2.5f); // Adjust this offset as necessary
    camera->position = position + front * cameraOffset.z + glm::vec3(0.0f, cameraOffset.y, 0.0f); // Use the front vector for direction
}

void Player::setPositionFromModelMatrix() {
    position = glm::vec3(model[3]);  // Extracts the translation part of the model matrix
}

// General method to draw any player
void Player::draw(Camera* camera) {
    playerShader->Activate();
    // Set the view and projection matrices
    camera->matrix(45.0f, 0.1f, 300.0f, playerShader, "camMatrix");
    glUniformMatrix4fv(glGetUniformLocation(playerShader->ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
    // Common rendering code
    playerTexture->Bind();
    playerVAO->Bind();
    glDrawElements(GL_TRIANGLES, indices_size, GL_UNSIGNED_INT, 0);
    playerVAO->Unbind();
}

void Player::drawRemote(){
    playerShader->Activate();
    glUniformMatrix4fv(glGetUniformLocation(playerShader->ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
    playerTexture->Bind();
    playerVAO->Bind();
    glDrawElements(GL_TRIANGLES, indices_size, GL_UNSIGNED_INT, 0);
    playerVAO->Unbind();
}

void Player::keyboard_input(GLFWwindow* window) {
	// Handles key inputs
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
		position += speed * orientation;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
		position += speed * -glm::normalize(glm::cross(orientation, up));
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
		position += speed * -orientation;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
		position += speed * glm::normalize(glm::cross(orientation, up));
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS){
		position += speed * up;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS){
		position += speed * -up;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS){
		speed = 0.1f;
	}
	else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE){
		speed = 0.01f;
	}
}

void Player::updateOrientation() {
    // Update the player model's orientation
    glm::vec3 front;
    front.x = cos(glm::radians(yaw));
    front.z = sin(glm::radians(yaw));
    orientation = glm::normalize(front);
}

void Player::mouse_input(GLFWwindow* window, Camera* camera) {
    int width, height;
    glfwGetWindowSize(window, &width, &height);

    // Check if the left mouse button is pressed
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);

        float rotX = sensitivity * (float)(mouseY - (height / 2)); // Vertical movement
        float rotY = sensitivity * (float)(mouseX - (width / 2));  // Horizontal movement

        yaw += rotY;
        pitch += rotX;

        // Clamp pitch to avoid gimbal lock (prevent looking straight up or down)
        if (pitch > 0) pitch = 0;
        if (pitch < -15.0f) pitch = -15.0f;

        // Update player orientation based on yaw
        updateOrientation();

        // Update camera orientation based on pitch
        camera->updateCameraOrientation(yaw, pitch);

        // Reset cursor position
        glfwSetCursorPos(window, (width / 2), (height / 2));

    } else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
        // Unhide the cursor when LMB is released
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
}

// void Player::mouse_input(GLFWwindow* window) {
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
