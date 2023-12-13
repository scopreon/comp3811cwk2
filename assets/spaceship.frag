#version 430


in vec3 v2fColor;
in vec3 v2fNormal;


layout (location = 0) out vec3 oColor;
// layout (location = 2) uniform vec4 uColor;


layout(location = 5) uniform vec3 uLightDir;
layout(location = 6) uniform vec3 uLightDiffuse;
layout(location = 7) uniform vec3 uSceneAmbient;

void main()
{

    vec3 normal = normalize(v2fNormal);
    float nDotL = max(0.0, dot(normal, uLightDir));

    vec3 diffuseColor = uSceneAmbient + nDotL * uLightDiffuse;
	oColor = v2fColor * diffuseColor;
}
