#version 330

uniform sampler2D map0;

in	vec4 fs_color;
in  vec2 fs_texcoord;

void main ()
{
	gl_FragColor = texture(map0, fs_texcoord) + (fs_color * 0.0);
}