#version 330 core
layout(location = 0) in vec3 tPos;  // Input vertex position
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec2 aTexOffset;

out vec2 texCoord; // Output texture coordinates to fragment shader
out vec2 texOffset;

uniform mat4 camMatrix;

void main() {
    gl_Position = camMatrix * vec4(tPos, 1.0);
    texCoord = aTexCoord;
    texOffset = aTexOffset;
}