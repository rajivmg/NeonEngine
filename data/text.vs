#version 330

uniform mat4 proj;

in vec3 in_position;
in vec2 in_texcoord;
in vec4 in_color;

out vec2 texcoord;
out vec4 color;

void main()
{
	gl_Position = proj * vec4(in_position, 1.0);
	color = in_color;
	texcoord = in_texcoord;
}