#version 430

layout (location = 0) out vec4 oColor;

layout (location = 2) uniform vec4 uColor;

void main()
{
	oColor = uColor;
}
