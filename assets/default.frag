#version 430
in vec3 v2fColor;
layout( location = 0 ) uniform vec3 uBaseColor;
layout( location = 0 ) out vec3 oColor;

layout( location = 5 ) uniform vec3 uLightDir; 
layout( location = 6 ) uniform vec3 uLightDiffuse;
layout( location = 7 ) uniform vec3 uSceneAmbient;
in vec3 v2fNormal;
void main()
{vec3 normal = normalize(v2fNormal);
float nDotL = max( 0.0, dot( normal, uLightDir ) );
oColor = (uSceneAmbient + nDotL * uLightDiffuse) * v2fColor;}
