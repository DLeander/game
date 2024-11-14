#version 330 core
out vec4 FragColor;  // Output color of the fragment

in vec3 color;  // Input color from vertex shader
in vec2 texCoord;

uniform sampler2D texture1;  // Texture sampler

void main() {
    // FragColor = vec4(color, 1.0);  // Set the fragment color
    FragColor = texture(texture1, texCoord);  // Set the fragment color to the texture color
}