#version 330

uniform sampler2D map0;

in	vec4 OutColor;
in  vec2 OutTexcoord;

// out vec4 out_color;

void main ()
{
	gl_FragColor = texture(map0, OutTexcoord) + (OutColor * 0.0);
}