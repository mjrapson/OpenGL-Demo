#version 450 core

layout(binding = 1) uniform samplerCube skyboxCubemap;

in vec3 fragDirection;
out vec4 FragColor;

void main()
{
    vec3 dir = normalize(fragDirection);
    vec3 color = texture(skyboxCubemap, dir).rgb;
    FragColor = vec4(color, 1.0);
}
