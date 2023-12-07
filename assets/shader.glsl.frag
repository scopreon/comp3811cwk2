#version 430

layout (location = 0) out vec4 o_Color;

layout (location = 2) uniform vec4 u_Color;

void main()
{
	o_Color = u_Color;
}
