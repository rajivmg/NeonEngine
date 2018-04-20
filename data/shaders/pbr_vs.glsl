#version 330

layout(location = 0) in vec3 VertP;
layout(location = 1) in vec3 VertN;
layout(location = 2) in vec2 VertUV;

uniform mat4 Projection;
uniform mat4 View;

out VS_OUT
{
	smooth vec2 VertUV;
	smooth vec3 VertN;
} vs_out;

void main()
{
	gl_Position = Projection * View * vec4(VertP, 1.0);
	
	vs_out.VertUV = VertUV;
	vs_out.VertN  = VertN;
}