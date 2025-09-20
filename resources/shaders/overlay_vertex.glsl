#version 430 core

layout (location = 0) in vec3 vertexPosition;

layout(std140, binding = 0) uniform TransformBlock {
    mat4 projection;
    mat4 view;
    mat4 model;
};

void main()
{
    gl_Position = projection * view * model * vec4(vertexPosition, 1.0);
}
