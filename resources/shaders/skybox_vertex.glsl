#version 450 core

layout(std140, binding = 0) uniform TransformBlock {
    mat4 projection;
    mat4 view;
    mat4 model;
};

layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec2 vertexTextureUV;
layout (location = 2) in vec3 vertexNormal;

out vec3 vertexDirection;

void main()
{
    vec4 position = mat4(mat3(view)) * vec4(vertexPosition, 1.0);
    vertexDirection = vertexPosition;
    
    gl_Position = projection * position;
}
