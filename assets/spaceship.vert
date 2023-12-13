#version 430

layout (location = 0) in vec3 iPosition;

layout ( location = 0 ) uniform mat4 uProjCameraWorld;
layout ( location = 1 ) uniform mat4 uTransform;

void main()
{
	gl_Position = uProjCameraWorld * uTransform * vec4(iPosition, 1.0);
}
