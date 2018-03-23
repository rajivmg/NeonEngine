#version 330

uniform sampler2D Sampler0;

in VS_OUT
{
	smooth vec2 VertUV;
	smooth vec4 VertColor;
} fs_in;

out vec4 OutColor;

void main()
{
	//OutColor = vec4( pow(fs_in.VertColor.rgb, vec3(2.2)), fs_in.VertColor.a);

	OutColor = texture(Sampler0, fs_in.VertUV) * fs_in.VertColor;

}