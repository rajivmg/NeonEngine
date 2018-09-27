#version 330

in VS_OUT
{
    smooth vec4 Color;
} ps_in;

out vec4 OutColor;

void main()
{
    OutColor = ps_in.Color;
}