#version 430

layout(location = 0) out vec3 oColor;

layout(location = 5) uniform vec3 uLightDir;
layout(location = 6) uniform vec3 uLightDiffuse;
layout(location = 7) uniform vec3 uSceneAmbient;
layout(location = 8) uniform vec3 uCameraPosWorld;
layout(location = 9) uniform vec3 uLightPosWorld;

layout(location = 10) uniform vec3 uMaterialSpec;
layout(location = 11) uniform float uMaterialShininess;
layout(location = 12) uniform vec3 uMaterialEmissive;

layout(location = 13) uniform vec3 uPointLightPos;
layout(location = 14) uniform vec3 uPointLightColor;
layout(location = 15) uniform float uPointLightAttenuation;

in vec3 v2fColor;
in vec3 v2fNormal;
in vec2 v2fTexCoord;

layout(binding = 0) uniform sampler2D uTexDiffuse;

in vec3 v2fWorldPos;

void main()
{

    // Calculate View Direction
    vec3 cameraPos = normalize(uCameraPosWorld - v2fWorldPos);

    // Calculate Light Direction
    vec3 lightDir = normalize(uLightPosWorld - v2fWorldPos);

    // Reflection Vector
    vec3 reflectDir = reflect(-lightDir, v2fNormal);

    // Diffuse Reflection
    float nDotL = max(0.0, dot(v2fNormal, lightDir));

    // Get Specular Component
    float spec = pow(max(dot(cameraPos, reflectDir), 0.0), uMaterialShininess * 4);

    // Check if a texture is bound
    vec3 textureColor = vec3(1.0); // Default to white if no texture
    if (texture(uTexDiffuse, v2fTexCoord).r > 0.0) {
        textureColor = texture(uTexDiffuse, v2fTexCoord).rgb;
    }

    vec3 ambient = uSceneAmbient * textureColor;

    vec3 diffuse = v2fColor * textureColor;

    vec3 specular = uMaterialSpec * textureColor * spec;

    vec3 emissive = uMaterialEmissive * textureColor;

    float distanceToPointLight = length(uPointLightPos - v2fWorldPos);
    float pointLightAttenuation = 1.0 / (1.0 + uPointLightAttenuation * pow(distanceToPointLight, 2.0));

    // Final Color
    oColor = ambient + pointLightAttenuation * (diffuse + specular) + emissive;

}
