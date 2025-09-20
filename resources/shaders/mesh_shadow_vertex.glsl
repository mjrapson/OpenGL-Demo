#version 430 core

layout (location = 0) in vec3 vertexPosition;

layout(std140, binding = 0) uniform LightTransformBlock {
    mat4 lightSpaceMatrix;
    mat4 modelMatrix;
};

void main()
{
    gl_Position = lightSpaceMatrix * modelMatrix * vec4(vertexPosition, 1.0);
}
