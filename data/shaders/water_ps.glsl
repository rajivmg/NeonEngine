#version 330
#extension GL_ARB_shading_language_420pack: require

uniform sampler2D Sampler0;
uniform sampler2D Sampler1;

layout(std140, binding = 0) uniform CgCommon
{
	float Time;
};

uniform float Magnitude = 0.02;

in VS_OUT
{
	smooth vec2 VertUV;
	smooth vec4 VertColor;
} fs_in;

out vec4 OutColor;

void main()
{
	vec2 DispUV = vec2(fs_in.VertUV.x + CgCommon.Time * 2, fs_in.VertUV.y + CgCommon.Time * 2);

	vec2 Disp = texture(Sampler1, DispUV).xy;
	Disp = (Disp * 2  - 1) * Magnitude; 
	OutColor = texture(Sampler0, fs_in.VertUV + Disp);
}