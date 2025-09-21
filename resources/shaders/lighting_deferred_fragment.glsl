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
    vec3 color = texture(colorTexture, fragmentTextureUV).rgb;
    vec3 normal = texture(normalTexture, fragmentTextureUV).rgb;

    // Normalize the normal vector
    normal = normalize(normal * 2.0 - 1.0);

    // Calculate diffuse lighting
    vec3 lightDir = normalize(dirLightDirection);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * dirLightDiffuseColor.rgb * color;

    // Calculate directional light shadows
    // Convert fragment position into light space
    vec3 fragmentWorldPos = texture(positionTexture, fragmentTextureUV).xyz;
    vec4 fragmentPositionInLightSpace = dirLightSpaceMatrix * vec4(fragmentWorldPos, 1.0);
    // Project light space fragment position into UV coordinates ([0, 1]) to lookup on shadow map texture
    vec3 projectedCoords = fragmentPositionInLightSpace.xyz / fragmentPositionInLightSpace.w;
    projectedCoords = projectedCoords * 0.5 + 0.5;
    float bias = max(0.005 * (1.0 - dot(normal, lightDir)), 0.0005);
    projectedCoords.z -= bias;
    
    //float shadow = texture(directionalShadowMap, projectedCoords.xyz);
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(directionalShadowMap, 0);
    for(int x = -1; x <= 1; ++x) {
        for(int y = -1; y <= 1; ++y) {
            vec2 offset = vec2(x, y) * texelSize;
            shadow += texture(directionalShadowMap, vec3(projectedCoords.xy + offset, projectedCoords.z));
        }
    }
    shadow /= 9.0;

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

        totalPointLightColor += (1.0 - pointLightShadow) * pointLightDiffuse * attenuation;
        totalPointLightColor += pointLightDiffuse * attenuation;
    }

    // Final color calculation
    vec3 ambient = 0.2 * color;
    FragColor = vec4(diffuse + totalPointLightColor + ambient, 1.0);
}
