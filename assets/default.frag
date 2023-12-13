#version 430

in vec3 v2fColor;
layout(location = 0) uniform vec3 uBaseColor;
layout(location = 0) out vec3 oColor;

layout(location = 5) uniform vec3 uLightDir;
layout(location = 6) uniform vec3 uLightDiffuse;
layout(location = 7) uniform vec3 uSceneAmbient;

in vec3 v2fNormal;
in vec2 v2fTexCoord;

layout(binding = 0) uniform sampler2D uTexture;

void main()
{
    vec3 normal = normalize(v2fNormal);
    float nDotL = max(0.0, dot(normal, uLightDir));

    vec3 diffuseColor = uSceneAmbient + nDotL * uLightDiffuse;

    // Check if a texture is bound
    vec3 textureColor = vec3(1.0); // Default to white if no texture
    if (texture(uTexture, v2fTexCoord).r > 0.0) {
        textureColor = texture(uTexture, v2fTexCoord).rgb;
    }

    oColor = diffuseColor * v2fColor * textureColor;
    // * textureColor;
}
