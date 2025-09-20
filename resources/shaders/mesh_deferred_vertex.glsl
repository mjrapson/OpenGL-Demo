#version 450 core

layout(std140, binding = 0) uniform TransformBlock {
    mat4 projection;
    mat4 view;
    mat4 model;
};

layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec2 vertexTextureUV;
layout (location = 2) in vec3 vertexNormal;

out vec3 fragmentPosition;
out vec2 fragmentTextureUV;
out vec3 fragmentNormal;

void main()
{
    // Calculate the fragment position in world space
    fragmentPosition = vec3(model * vec4(vertexPosition, 1.0));

    gl_Position = projection * view * model * vec4(vertexPosition, 1.0);

    // Pass normal and texture coordinates to fragment shader
    fragmentNormal = mat3(transpose(inverse(model))) * vertexNormal;
    fragmentTextureUV = vertexTextureUV;
}
