#version 430

in vec3 v2fColor;
layout(location = 0) uniform vec3 uBaseColor;
layout(location = 0) out vec3 oColor;

layout(location = 5) uniform vec3 uLightDir;
layout(location = 6) uniform vec3 uLightDiffuse;
layout(location = 7) uniform vec3 uSceneAmbient;
layout(location = 8) uniform vec3 uViewDir;

in vec3 v2fviewPos;
in vec3 v2fNormal;
in vec2 v2fTexCoord;

layout(binding = 0) uniform sampler2D uTexture;

uniform vec3 pointLights[3];
uniform vec3 pointColor[3];
uniform float attenuation[3];

vec3 calculateDirect(vec3 normal) {

    // Diffuse Reflection
    float nDotL = max(0.0, dot(normal, uLightDir));

    // Diffuse Final = Reflectance * Incoming Diffuse Light
    vec3 diffuseColor =  nDotL * uLightDiffuse;

    // Halfway Direction Vector
    vec3 halfwayDir = normalize(uLightDir + uViewDir);

    float specLight = pow(max(dot(normal, halfwayDir), 0.0f), 2);

    vec3 specColor = specLight * uLightDiffuse;

    vec3 direct = diffuseColor + specColor;

    return direct;
}

vec3 calculatePoint(int index, vec3 normal) {

    vec3 pLightDir = normalize(pointLights[index] - v2fviewPos);
    float distance = length(pointLights[index] - v2fviewPos);

    float nDotL = max(0.0, dot(normal, pLightDir));

    vec3 diffuseColor = nDotL * pointColor[index] / (attenuation[index] * distance);

    vec3 viewDir = normalize(uViewDir);
    vec3 halfwayDir = normalize(uLightDir + uViewDir);
    float specLight = pow(max(dot(normal, halfwayDir), 0.0f), 2);
    vec3 specColor = specLight * pointColor[index] / (attenuation[index] * distance);

    return diffuseColor + specColor;
}


void main()
{

    // Normal Vector
    vec3 normal = normalize(v2fNormal);

    vec3 ambient = uSceneAmbient * v2fColor;
    
    vec3 dirLight = calculateDirect(normal);

    vec3 pointLightColor = vec3(0.0);

    for (int i = 0; i < 3; i++) {
        pointLightColor += calculatePoint(i, normal);
    }

    vec3 finalColor = ambient + dirLight + pointLightColor;

    // Check if a texture is bound
    vec3 textureColor = vec3(1.0); // Default to white if no texture
    if (texture(uTexture, v2fTexCoord).r > 0.0) {
        textureColor = texture(uTexture, v2fTexCoord).rgb;
    }

    // oColor = finalColor * v2fColor * textureColor;

    oColor = (ambient + dirLight) * v2fColor * textureColor;

}
