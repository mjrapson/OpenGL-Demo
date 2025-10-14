#version 450 core

layout(location = 0) in vec2 vertexPosition;

out vec3 fragDirection;

layout(std140, binding = 0) uniform TransformBlock {
    mat4 projection;
    mat4 view;
};

void main()
{
    vec4 clipPos = vec4(vertexPosition, 0.0, 1.0);

    vec4 viewPos = inverse(projection) * clipPos;
    viewPos /= viewPos.w;

    fragDirection = (mat3(view) * normalize(viewPos.xyz));
    gl_Position = vec4(vertexPosition, 0.0, 1.0); // full screen quad
}
