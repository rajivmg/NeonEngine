#version 330

in vec3 in_position;
in vec4 in_color;

out vec4 color;

uniform mat4 proj;

void main ()
{
  	gl_Position = proj * vec4(in_position, 1.0);

  	color = in_color;
}