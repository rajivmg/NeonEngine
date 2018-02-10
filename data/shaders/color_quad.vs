#version 330

uniform mat4 projection;

in 	vec3 vs_pos;
in 	vec4 vs_color;
out vec4 fs_color;

void main ()
{
  	gl_Position = projection * vec4(vs_pos, 1.0);
  	fs_color = vec4(pow(vs_color.rgb, vec3(2.2)), vs_color.a);
}