#version 430 core

in vec4 fragmentPositionLightSpace;
in vec3 fragmentPositionWorld;

layout(std140, binding = 1) uniform LightPositionBlock {
    vec3 lightPosition;
};

layout(std140, binding = 2) uniform FarPlaneBlock {
    float farPlane;
};

void main()
{
    float lightDistance = length(fragmentPositionWorld - lightPosition);
    lightDistance = lightDistance / farPlane;
    gl_FragDepth = lightDistance;  // Store radial depth in depth buffer
}
