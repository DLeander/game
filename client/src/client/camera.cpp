#include "camera.h"

CCAMERA::CCAMERA(int iWidth, int iHeight, glm::vec3 v3Position) {
    m_iWidth = iWidth;
    m_iHeight = iHeight;
    m_v3Position = v3Position;
}

void CCAMERA::matrix(float fFOVdeg, float fNearPlane, float fFarPlane, CSHADER* shader, const char* uniform) {
    m_m4View = glm::mat4(1.0f);
    m_m4Projection = glm::mat4(1.0f);

    m_m4View = glm::lookAt(m_v3Position, m_v3Position + m_v3Orientation, m_v3Up);
    m_m4Projection = glm::perspective(glm::radians(fFOVdeg), (float)m_iWidth / (float)m_iHeight, fNearPlane, fFarPlane);

    glUniformMatrix4fv(glGetUniformLocation(shader->m_ID, uniform), 1, GL_FALSE, glm::value_ptr(m_m4Projection*m_m4View));
}

void CCAMERA::updateCameraOrientation(float fYaw, float fPitch) {
    // Calculate the front vector for the camera based on the player's yaw and pitch
    glm::vec3 v3Front;
    v3Front.x = cos(glm::radians(fYaw)) * cos(glm::radians(fPitch));
    v3Front.y = sin(glm::radians(-fPitch));
    v3Front.z = sin(glm::radians(fYaw)) * cos(glm::radians(fPitch));
    m_v3Orientation = glm::normalize(v3Front);
}

// void Player::updateOrientation() {
//     // Update the player model's orientation
//     glm::vec3 front;
//     front.x = cos(glm::radians(yaw));
//     front.z = sin(glm::radians(yaw));
//     orientation = glm::normalize(front);
// }

//---------------------------------------Redundant but may have some interesting code to look at later on.----------------------------------------------
// void Camera::matrix(float FOVdeg, float nearPlane, float farPlane, Shader* shader, glm::mat4 model) {
//     // Initialize view and projection matrices
//     view = glm::mat4(1.0f);
//     projection = glm::mat4(1.0f);

//     // Set up the view matrix to look from the camera's position, with orientation and up vector
//     view = glm::lookAt(position, position + orientation, up);

//     // Set up the perspective projection matrix
//     projection = glm::perspective(glm::radians(FOVdeg), (float)width / (float)height, nearPlane, farPlane);

//     // Calculate model position relative to the camera's orientation
//     glm::vec3 modelOffset = glm::vec3(0.0f, 0.5f, 2.5f); // Adjust offset as needed
//     glm::vec3 front = glm::normalize(orientation); // Front direction of the camera
//     glm::vec3 right = glm::normalize(glm::cross(front, up)); // Right direction of the camera
//     glm::vec3 upAdjusted = glm::cross(front, right); // Adjusted up vector
	
// 	// Print camera positon:
//     // Calculate the model position using camera position and offsets
//     glm::vec3 modelPosition = position + (front * modelOffset.z) + (upAdjusted * modelOffset.y) + (right * modelOffset.x);
    
//     // Create the model matrix
//     model = glm::translate(glm::mat4(1.0f), modelPosition);

//     // Rotate the model based on the camera's orientation
//     float cameraRotationAngle = glm::degrees(atan2(orientation.z, orientation.x));
//     model = glm::rotate(model, glm::radians(-cameraRotationAngle), up); // Rotate based on camera's orientation

//     // Pass the matrices to the shader
//     glUniformMatrix4fv(glGetUniformLocation(shader->ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
//     glUniformMatrix4fv(glGetUniformLocation(shader->ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
//     glUniformMatrix4fv(glGetUniformLocation(shader->ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
// }

// void Camera::inputs(GLFWwindow* window){
// 	// Handles mouse inputs
// 	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
// 	{
// 		// Hides mouse cursor
// 		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

// 		// Prevents camera from jumping on the first click
// 		if (firstClick)
// 		{
// 			glfwSetCursorPos(window, (width / 2), (height / 2));
// 			firstClick = false;
// 		}

// 		// Stores the coordinates of the cursor
// 		double mouseX;
// 		double mouseY;
// 		// Fetches the coordinates of the cursor
// 		glfwGetCursorPos(window, &mouseX, &mouseY);

// 		// Normalizes and shifts the coordinates of the cursor such that they begin in the middle of the screen
// 		// and then "transforms" them into degrees 
// 		float rotX = sensitivity * (float)(mouseY - (height / 2)) / height;
// 		float rotY = sensitivity * (float)(mouseX - (width / 2)) / width;

// 		// Calculates upcoming vertical change in the orientation
// 		glm::vec3 neworientation = glm::rotate(orientation, glm::radians(-rotX), glm::normalize(glm::cross(orientation, up)));

// 		// Decides whether or not the next vertical orientation is legal or not
// 		if (abs(glm::angle(neworientation, up) - glm::radians(90.0f)) <= glm::radians(85.0f))
// 		{
// 			orientation = neworientation;
// 		}

// 		// Rotates the orientation left and right
// 		orientation = glm::rotate(orientation, glm::radians(-rotY), up);

// 		// Sets mouse cursor to the middle of the screen so that it doesn't end up roaming around
// 		glfwSetCursorPos(window, (width / 2), (height / 2));
// 	}
// 	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
// 	{
// 		// Unhides cursor since camera is not looking around anymore
// 		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
// 		// Makes sure the next time the camera looks around it doesn't jump
// 		firstClick = true;
// 	}
// }