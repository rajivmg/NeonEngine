#version 330

uniform mat4 projection;

in vec3 vs_pos;
in vec4 vs_color;
in vec2 vs_texcoord;

out vec4 fs_color;
out vec2 fs_texcoord;

void main ()
{
  	gl_Position = projection * vec4(vs_pos, 1.0);

  	fs_color = vs_color;
	fs_texcoord = vs_texcoord;
}