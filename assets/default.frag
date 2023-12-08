#version 430

layout(location = 0) uniform vec3 uBaseColor;
layout(location = 0) out vec3 oColor;

layout(location = 5) uniform vec3 uLightDir;
layout(location = 6) uniform vec3 uLightDiffuse;
layout(location = 7) uniform vec3 uSceneAmbient;
layout(location = 8) uniform vec3 uViewPos;
layout(location = 9) uniform vec3 uLightPos;

in vec3 v2fColor;
in vec3 v2fNormal;
in vec2 v2fTexCoord;

layout(binding = 0) uniform sampler2D uTexture;

in vec3 FragPos;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

uniform Material material;

void main()
{
    vec3 ambient = v2fColor * material.ambient;

    // Normal Vector
    vec3 normal = normalize(v2fNormal);

    // Calculate View Direction
    vec3 viewDir = normalize(uViewPos - FragPos);

    // Calculate Light Direction
    vec3 lightDir = normalize(uLightPos - FragPos);

    // Reflection Vector
    vec3 reflectDir = reflect(-lightDir, normal);

    // Get Specular Component
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = v2fColor * (spec * material.specular);

    // Diffuse Reflection
    float nDotL = max(0.0, dot(normal, lightDir));

    // Diffuse Final = Reflectance * Incoming Diffuse Light
    vec3 diffuseColor = (nDotL * material.diffuse) * v2fColor;



    // Check if a texture is bound
    vec3 textureColor = vec3(1.0); // Default to white if no texture
    if (texture(uTexture, v2fTexCoord).r > 0.0) {
        textureColor = texture(uTexture, v2fTexCoord).rgb;
    }

    // Final Color
    oColor = (diffuseColor + uSceneAmbient + specular) * v2fColor * textureColor;

}
