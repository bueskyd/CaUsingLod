#version 330 core

layout(location = 0) in vec2 inVertexPosition;
layout(location = 1) in vec2 offset;
layout(location = 2) in vec3 inColor;

out vec3 fragColor;

layout (std140) uniform Matrices
{
    mat4 viewProjection;
    mat4 absoluteOrtho;
    mat4 relativeOrtho;
    float cameraX;
    float cameraY;
    float cameraZ;
};

void main()
{
    float x = inVertexPosition.x + offset.x;
    float y = inVertexPosition.y + offset.y;
    vec4 position = vec4(x, y, -0.75f, 1.0f);
    gl_Position = absoluteOrtho * position;

    fragColor = inColor;
}
