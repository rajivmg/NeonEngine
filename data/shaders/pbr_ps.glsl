#version 330

uniform sampler2D Sampler0;

in VS_OUT
{
	smooth vec2 VertUV;
	smooth vec3 VertN;
} fs_in;

out vec4 OutColor;

void main()
{
	OutColor = vec4(fs_in.VertN, 1.0) + texture(Sampler0, fs_in.VertUV) * 0.0;
}