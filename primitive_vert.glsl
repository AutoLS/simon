#version 330 core

layout (location = 0) in vec3 InPos;

out vec4 TexCoords;

uniform mat4 Transform;
uniform mat4 View;
uniform mat4 Projection;

void main()
{
	gl_Position = Projection * View * Transform * vec4(InPos, 1.0);
}