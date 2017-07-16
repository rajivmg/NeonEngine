#version 330

uniform mat4 ProjMatx;

in vec3 InPos;
in vec4 InColor;

out vec4 OutColor;

void main ()
{
  	gl_Position = ProjMatx * vec4(InPos, 1.0);

  	OutColor = InColor;
}