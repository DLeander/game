#version 330 core
layout(location = 0) in vec3 tPos;  // Input vertex position
layout(location = 1) in vec3 tColor; // Input vertex color

out vec3 color;  // Output color to fragment shader


uniform mat4 camMatrix;


void main() {
    gl_Position = camMatrix * vec4(tPos, 1.0);
    color = tColor;  // Pass color to fragment shader
}