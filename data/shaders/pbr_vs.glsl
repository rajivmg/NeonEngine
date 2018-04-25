#version 330

layout(location = 0) in vec3 VertP;
layout(location = 1) in vec3 VertN;
layout(location = 2) in vec2 VertUV;

uniform mat4 Projection;
uniform mat4 View;

out VS_OUT
{
	vec2 VertUV;
	vec3 N;
	vec3 L;
	vec3 V;
} vs_out;

void main()
{
	// vec3 LightPos = mat3(View) * vec3(75.0, 327.0, -7.0);

	const vec3 LightPos = vec3(0.0, 150.0, 80.0);

	vec4 P = View * vec4(VertP, 1.0);

	vs_out.N = mat3(View) * VertN;

	vs_out.L = LightPos - P.xyz;

	vs_out.V = -P.xyz;

	gl_Position = Projection * P;
	
	vs_out.VertUV = VertUV;
}