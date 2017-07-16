#version 330

uniform sampler2D map0;

in vec2 OutTexcoord;
in vec4 OutColor;

void main()
{
	gl_FragColor = vec4(1.0, 1.0, 1.0, texture(map0, OutTexcoord).a) * OutColor;
}