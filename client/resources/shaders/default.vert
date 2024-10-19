#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTex;

out vec3 color;
out vec2 texCoord;

uniform mat4 camMatrix;     // Projection matrix times the view matrix.
uniform mat4 model;        // Model matrix for object-specific transformations

void main()
{
    // Apply the model, view, and projection matrices to position the object relative to the camera
    gl_Position = camMatrix * model * vec4(aPos, 1.0);

    color = aColor;
    texCoord = aTex;
}