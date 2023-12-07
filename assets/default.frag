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

void main()
{
    // Diffuse  + Ambient Contribution

    // Normal Vector
    vec3 normal = normalize(v2fNormal);

    // Diffuse Reflection
    float nDotL = max(0.0, dot(normal, uLightDir));

    // Diffuse Final = Reflectance * Incoming Diffuse Light
    vec3 diffuseColor = uSceneAmbient + nDotL * uLightDiffuse;


    // Specular Contribution

    // Halfway Direction Vector
    vec3 halfwayDir = normalize(uLightDir + uViewDir);

    // Get Dot Product of Normal and Halfway Vectors
    float specLight = pow(max(dot(normal, halfwayDir), 0.0f), 2);


    // Check if a texture is bound
    vec3 textureColor = vec3(1.0); // Default to white if no texture
    if (texture(uTexture, v2fTexCoord).r > 0.0) {
        textureColor = texture(uTexture, v2fTexCoord).rgb;
    }

    // Final output colour
    oColor = diffuseColor * v2fColor * textureColor;

}

