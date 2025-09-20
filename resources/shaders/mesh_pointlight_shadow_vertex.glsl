#version 430 core

layout (location = 0) in vec3 vertexPosition;

layout(std140, binding = 0) uniform LightTransformBlock {
    mat4 lightSpaceMatrix;
    mat4 modelMatrix;
};

out vec4 fragmentPositionLightSpace;
out vec3 fragmentPositionWorld;


void main()
{
    fragmentPositionLightSpace = lightSpaceMatrix * modelMatrix * vec4(vertexPosition, 1.0);
    gl_Position = fragmentPositionLightSpace;

    fragmentPositionWorld = (modelMatrix * vec4(vertexPosition, 1.0)).xyz;
}
