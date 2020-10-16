#version 330 core

layout (location = 0) in vec3 InPos;
layout (location = 1) in vec3 InTexCoords;

out vec3 TexCoords;
out vec3 Vertex;
out vec3 FragPos;

uniform mat4 Transform;
uniform mat4 View;
uniform mat4 Projection;

void main()
{
	FragPos = vec3(Transform * vec4(InPos, 1.0));
	gl_Position = Projection * View * vec4(FragPos, 1.0);
	TexCoords = InTexCoords;
}