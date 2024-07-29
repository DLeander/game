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
    // playerShader = Shader("resources/shaders/default.vert", "resources/shaders/default.frag");
    // Render the clients player
    GLfloat vertices[] = {
        // Cordiantes           // Colors
        -0.2f, -0.2f, 0.0f,     1.0f, 1.0f, 1.0f,
        -0.2f, 0.2f, 0.0f,      1.0f, 1.0f, 1.0f,
        0.2f, 0.2f, 0.0f,       1.0f, 1.0f, 1.0f,
        0.2f, -0.2f, 0.0f,      1.0f, 1.0f, 1.0f
    };

    GLuint indices[] = {
        0, 1, 3,
        1, 2, 3
    };
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
}

void Player::draw() {
    glm::mat4 transform = glm::mat4(1.0f);
    transform = glm::translate(transform, position);

    playerShader->Activate();
    // This should be stored in the player class so it can be used in the draw method and not have to be recalculated.
    glUniformMatrix4fv(playerShader->transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

    playerVAO->Bind();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
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
