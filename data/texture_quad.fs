#version 330

uniform sampler2D map0;

in	vec4 color;
in  vec2 texcoord;

out vec4 out_color;

void main ()
{
	out_color = texture(map0, texcoord) + (color * 0.0);
}