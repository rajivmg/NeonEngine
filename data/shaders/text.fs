#version 330

uniform sampler2D map0;

in vec4 fs_color;
in vec2 fs_texcoord;

void main()
{
	gl_FragColor = vec4(1.0, 1.0, 1.0, texture(map0, fs_texcoord).a) * fs_color;
}