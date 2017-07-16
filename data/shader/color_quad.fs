#version 330

#define GAMMA 2.2

in	vec4 OutColor;

void main ()
{
	gl_FragColor = vec4(pow(OutColor.rgb, vec3(GAMMA)), OutColor.a);
}