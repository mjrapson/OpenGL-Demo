#version 450

in vec3 vertexDirection;

out vec4 FragColor;

void main()
{
    FragColor = vec4(vertexDirection, 1.0);
}
