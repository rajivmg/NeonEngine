#version 330

in	vec4 fs_color;

void main ()
{
	gl_FragColor = vec4(pow(fs_color.rgb, vec3(2.2)), fs_color.a);
}