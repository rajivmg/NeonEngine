#version 330

uniform sampler2D map0;

in vec2 texcoord;
in vec4 color;

out vec4 out_color;

void main()
{
	out_color = vec4(1.0, 1.0, 1.0, texture(map0, texcoord).a) * color;
}