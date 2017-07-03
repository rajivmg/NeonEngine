#version 330

in vec3 in_position;
in vec4 in_color;
in vec2 in_texcoord;

out vec4 color;
out vec2 texcoord;

uniform mat4 proj;

void main ()
{
  	gl_Position = proj * vec4(in_position, 1.0);

  	color = in_color;
	texcoord = in_texcoord;
}