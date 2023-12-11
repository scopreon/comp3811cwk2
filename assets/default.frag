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

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Material material;
uniform Light light;

void main()
{

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

    // Diffuse Reflection
    float nDotL = max(0.0, dot(normal, lightDir));

    vec3 ambient = light.ambient * material.ambient;
    vec3 diffuse = light.diffuse * (nDotL * material.diffuse);
    vec3 specular = light.specular * (spec * material.specular);

    // Check if a texture is bound
    vec3 textureColor = vec3(1.0); // Default to white if no texture
    if (texture(uTexture, v2fTexCoord).r > 0.0) {
        textureColor = texture(uTexture, v2fTexCoord).rgb;
    }

    // Final Color
    oColor = (diffuse + ambient + specular) * v2fColor * textureColor;

}
