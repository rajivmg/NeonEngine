#version 330

uniform mat4 ProjMatx;

in vec3 InPos;
in vec4 InColor;
in vec2 InTexcoord;

out vec4 OutColor;
out vec2 OutTexcoord;

void main ()
{
  	gl_Position = ProjMatx * vec4(InPos, 1.0);

  	OutColor = InColor;
	OutTexcoord = InTexcoord;
}