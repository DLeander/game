#version 330 core
out vec4 FragColor;  // Output color of the fragment

in vec2 texCoord;
in vec2 texOffset;

uniform sampler2D texture1;  // Texture sampler

const float atlasColRowSize = 2.0;

void main() {
    vec2 atlasCoord = (texCoord / atlasColRowSize) + texOffset;
    FragColor = texture(texture1, atlasCoord);
}