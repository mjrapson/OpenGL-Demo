#version 430 core

layout(location = 0) in vec2 vertexPosition;
layout(location = 1) in vec2 vertexTextureUV;

out vec2 fragmentTextureUV;

void main()
{
    // Pass the texture coordinates to the fragment shader
    fragmentTextureUV = vertexTextureUV;

    // Set the position of the vertex in clip space
    gl_Position = vec4(vertexPosition, 0.0, 1.0);
}
