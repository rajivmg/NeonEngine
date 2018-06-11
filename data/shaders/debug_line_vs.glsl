#version 330

layout(location = 0) in vec3 P;
layout(location = 1) in vec4 Color;

uniform mat4 Projection;
uniform mat4 View;

out VS_OUT
{
    smooth vec4 Color;
} vs_out;

void main()
{
    gl_Position = Projection * View * vec4(P, 1.0);

    vs_out.Color = Color;
}