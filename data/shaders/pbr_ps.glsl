#version 330

uniform sampler2D Sampler0;

in VS_OUT
{
	vec2 VertUV;
	vec3 N;
	vec3 L;
	vec3 V;
} fs_in;

out vec4 OutColor;

void main()
{
	const vec3 DiffuseAlbedo = vec3(0.5, 0.2, 0.7);
	const vec3 SpecularAlbedo = vec3(0.7);
	const float SpecularPower = 256.0;

	vec3 N = normalize(fs_in.N);
	vec3 L = normalize(fs_in.L);
	vec3 V = normalize(fs_in.V);

	vec3 R = reflect(-L, N);

	vec3 Diffuse = max(0.0, dot(N, L)) * DiffuseAlbedo;
	vec3 Specular = pow(max(dot(R, V), 0.0), SpecularPower) * SpecularAlbedo;

	OutColor = vec4(Diffuse + Specular, 1.0) +  texture(Sampler0, fs_in.VertUV) * 0.0;
}