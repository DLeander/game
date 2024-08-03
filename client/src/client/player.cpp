#include "client/player.h"

Player::Player(Shader* shader) {
    // Initialize the player's position
    playerShader = shader;
    init();
    initialised = true;
}

Player::Player(Shader* shader, playerInfo info){
    position = info.position;
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
}

void Player::init() {
    speed = 0.01f;
    position = glm::vec3(0.0f, 0.0f, 0.0f);
    // Render the clients player
    // GLfloat vertices[] = {
    //     // Cordiantes           // Colors
    //     -0.2f, -0.2f, 0.0f,     1.0f, 1.0f, 1.0f,
    //     -0.2f, 0.2f, 0.0f,      1.0f, 1.0f, 1.0f,
    //     0.2f, 0.2f, 0.0f,       1.0f, 1.0f, 1.0f,
    //     0.2f, -0.2f, 0.0f,      1.0f, 1.0f, 1.0f
    // };

    // GLuint indices[] = {
    //     0, 1, 3,
    //     1, 2, 3
    // };

    GLfloat vertices[] = {
        // Positions            // Colors
        // Front face
        -0.2f, -0.2f,  0.2f,    1.0f, 1.0f, 1.0f,
        0.2f, -0.2f,  0.2f,    1.0f, 1.0f, 1.0f,
        0.2f,  0.2f,  0.2f,    1.0f, 1.0f, 1.0f,
        -0.2f,  0.2f,  0.2f,    1.0f, 1.0f, 1.0f,
        // Back face
        -0.2f, -0.2f, -0.2f,    1.0f, 1.0f, 1.0f,
        0.2f, -0.2f, -0.2f,    1.0f, 1.0f, 1.0f,
        0.2f,  0.2f, -0.2f,    1.0f, 1.0f, 1.0f,
        -0.2f,  0.2f, -0.2f,    1.0f, 1.0f, 1.0f,
        // Left face
        -0.2f, -0.2f, -0.2f,    1.0f, 1.0f, 1.0f,
        -0.2f, -0.2f,  0.2f,    1.0f, 1.0f, 1.0f,
        -0.2f,  0.2f,  0.2f,    1.0f, 1.0f, 1.0f,
        -0.2f,  0.2f, -0.2f,    1.0f, 1.0f, 1.0f,
        // Right face
        0.2f, -0.2f, -0.2f,    1.0f, 1.0f, 1.0f,
        0.2f, -0.2f,  0.2f,    1.0f, 1.0f, 1.0f,
        0.2f,  0.2f,  0.2f,    1.0f, 1.0f, 1.0f,
        0.2f,  0.2f, -0.2f,    1.0f, 1.0f, 1.0f,
        // Top face
        -0.2f,  0.2f, -0.2f,    1.0f, 1.0f, 1.0f,
        -0.2f,  0.2f,  0.2f,    1.0f, 1.0f, 1.0f,
        0.2f,  0.2f,  0.2f,    1.0f, 1.0f, 1.0f,
        0.2f,  0.2f, -0.2f,    1.0f, 1.0f, 1.0f,
        // Bottom face
        -0.2f, -0.2f, -0.2f,    1.0f, 1.0f, 1.0f,
        -0.2f, -0.2f,  0.2f,    1.0f, 1.0f, 1.0f,
        0.2f, -0.2f,  0.2f,    1.0f, 1.0f, 1.0f,
        0.2f, -0.2f, -0.2f,    1.0f, 1.0f, 1.0f
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
	playerVAO->LinkAttrib(*playerVBO, 0, 3, GL_FLOAT, 6 * sizeof(float), (void*)0);
	playerVAO->LinkAttrib(*playerVBO, 1, 3, GL_FLOAT, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    // Unbind all to prevent accidentally modifying them
    playerVAO->Unbind();
    playerVBO->Unbind();
    playerEBO->Unbind();

    // Init model, view, and projection matrices
    model = glm::mat4(1.0f);
    view = glm::mat4(1.0f);
    projection = glm::mat4(1.0f);
}

void Player::draw(int width, int height) {
    // transform = glm::translate(transform, position);
    view = glm::translate(view, glm::vec3(0.0f, -0.5f, -2.0f));
    projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
    playerShader->Activate();

    glUniformMatrix4fv(playerShader->modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(playerShader->viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(playerShader->projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
    

    playerVAO->Bind();
    glDrawElements(GL_TRIANGLES, indices_size, GL_UNSIGNED_INT, 0);
    playerVAO->Unbind();
}

void Player::keyboard_input(GLFWwindow* window) {
    float xOffset = 0.2f;
    float yOffset = 0.2f;

    if (position.y < 1 - yOffset && glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        position.y += speed;
    }
    if (position.y > -1 + yOffset && glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        position.y -= speed;
    }
    if (position.x < 1 - xOffset && glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        position.x += speed;
    }
    if (position.x > -1 + xOffset && glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        position.x -= speed;
    }
}

void Player::mouse_input(GLFWwindow* window) {

}
