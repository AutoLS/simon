#version 330 core

out vec4 FragColor;

in vec3 TexCoords;
in vec3 FragPos;

uniform vec4 Color;
uniform sampler2D Texture;

void main()
{
	vec4 TexColor = texture(Texture, TexCoords.xy) * Color;
	if(TexColor.a < 0.1)
		discard;
	FragColor = TexColor;
}