#version 430

layout( location = 0 ) uniform vec3 uBaseColor;

layout( location = 0 ) out vec3 oColor;

in vec3 v2fColor;

void main() {

    oColor = uBaseColor * v2fColor;

}