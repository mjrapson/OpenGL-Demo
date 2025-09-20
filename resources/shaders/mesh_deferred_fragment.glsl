#version 430 core

in vec3 fragmentPosition;
in vec2 fragmentTextureUV;
in vec3 fragmentNormal;

layout(location = 0) out vec4 fragColour;
layout(location = 1) out vec3 fragNormal;
layout(location = 2) out vec3 fragPosition;
out float fragDepth;

layout(binding = 1) uniform sampler2D diffuseTexture;

layout(std140, binding = 2) uniform MaterialBlock {
    int hasTexture;
    vec4 diffuseColour;      // offset 16
} material;


void main()
{
    if(material.hasTexture == 1)
    {
        fragColour = texture(diffuseTexture, fragmentTextureUV);
    }
    else
    {
        fragColour = material.diffuseColour;
    }

    // Output the normal (in view space)
    fragNormal = normalize(fragmentNormal);

    // Output the depth (using the built-in gl_FragCoord.z)
    fragDepth = gl_FragCoord.z;

    fragPosition = fragmentPosition;
}
