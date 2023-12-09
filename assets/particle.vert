#version 430

layout (location = 0) in vec3 a_Position;

layout ( location = 0 ) uniform mat4 u_ViewProj;
layout ( location = 1 ) uniform mat4 u_Transform;

void main()
{
	gl_Position = u_ViewProj * u_Transform * vec4(a_Position, 1.0);
}
