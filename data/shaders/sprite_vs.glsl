#version 330

layout(location = 0) in vec3 VertP;
layout(location = 1) in vec2 VertUV;
layout(location = 2) in vec4 VertColor;

uniform mat4 Projection;
uniform mat4 View;

out VS_OUT
{
	smooth vec2 VertUV;
	smooth vec4 VertColor;
} vs_out;

void main()
{
	gl_Position = Projection * View * vec4(VertP, 1.0);
	
	vs_out.VertUV = VertUV;
	vs_out.VertColor = VertColor;
}