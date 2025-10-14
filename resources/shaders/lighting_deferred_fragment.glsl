#version 430 core

struct PointLight
{
    vec3 position;
    vec3 color;
    float radius;
};

in vec2 fragmentTextureUV;

layout(binding = 0) uniform sampler2D colorTexture;
layout(binding = 1) uniform sampler2D normalTexture;
layout(binding = 2) uniform sampler2D positionTexture;
layout(binding = 3) uniform sampler2DShadow directionalShadowMap;
layout(binding = 4) uniform samplerCubeArray pointLightShadowMap;

layout(std140, binding = 5) uniform DirectionalLightBlock {
    vec3 dirLightDirection;
    vec4 dirLightDiffuseColor;
    mat4 dirLightSpaceMatrix;
};

layout(std140, binding = 6) uniform FarPlaneBlock {
    float farPlane;
};

#define MAX_POINT_LIGHTS 32
layout(std140, binding = 7) uniform PointLightBlock {
    int numberOfPointLights;
    PointLight pointLights[MAX_POINT_LIGHTS];
};

out vec4 FragColor;

void main()
{
    // Sample the G-buffer textures
    vec4 texels = texture(colorTexture, fragmentTextureUV);
    float alpha = texels.a;
    if(alpha < 0.1)
    {
        FragColor = vec4(0.0, 0.0, 0.0, 0.0);
        return;
    }

    vec3 color = texels.rgb;
    vec3 normal = normalize(texture(normalTexture, fragmentTextureUV).rgb);

    // Normalize the normal vector

    // Calculate diffuse lighting
    vec3 lightDir = normalize(-dirLightDirection);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * dirLightDiffuseColor.rgb * color;

    // Calculate directional light shadows
    // Convert fragment position into light space
    vec3 fragmentWorldPos = texture(positionTexture, fragmentTextureUV).xyz;
    vec4 fragmentPositionInLightSpace = dirLightSpaceMatrix * vec4(fragmentWorldPos, 1.0);

    // Project light space fragment position into UV coordinates ([0, 1]) to lookup on shadow map texture
    vec3 projectedCoords = fragmentPositionInLightSpace.xyz / fragmentPositionInLightSpace.w;
    projectedCoords = projectedCoords * 0.5 + 0.5;
    float bias = max(0.005 * (1.0 - dot(normal, lightDir)), 0.005);
    projectedCoords.z -= bias;
    
    float shadow = texture(directionalShadowMap, projectedCoords.xyz);
    diffuse *= shadow;

    // Calculate point light contributions
    vec3 totalPointLightColor = vec3(0.0);

    for(int i = 0; i < numberOfPointLights; ++i)
    {
        vec3 lightPos = pointLights[i].position;
        vec3 lightColor = pointLights[i].color;
        float lightRadius = pointLights[i].radius;

        // Direction from fragment to the light
        vec3 fragmentToLight = normalize(lightPos - fragmentWorldPos);
        float distanceToLight = length(lightPos- fragmentWorldPos);

        float lightFactor = max(dot(normal, fragmentToLight), 0.0);
        vec3 pointLightDiffuse = lightFactor * lightColor * color;

        // Calculate fall-off (decrease intensity with distance by inverse square law) up to the lights radius
        float attenuation = 1.0 / (1.0 + (distanceToLight / lightRadius) * (distanceToLight / lightRadius));

        // Calculate shadow
        vec3 lightToFragment = normalize(fragmentWorldPos - lightPos);
        float lightDepth = length(fragmentWorldPos - lightPos);

        // Sample the shadow cube map
        float closestDepth = texture(pointLightShadowMap, vec4(lightToFragment, i)).r * farPlane;

        // Adjust bias based on angle
        float pointlightBias = max(0.05 * (1.0 - dot(normal, lightToFragment)), 0.005);
        float pointLightShadow = 1.0; //(lightDepth - pointlightBias > closestDepth) ? 1.0 : 0.0;

        //totalPointLightColor += (1.0 - pointLightShadow) * pointLightDiffuse * attenuation;
        totalPointLightColor += pointLightDiffuse * attenuation;
    }

    // Final color calculation
    FragColor = vec4(diffuse + totalPointLightColor, 1.0);
}
