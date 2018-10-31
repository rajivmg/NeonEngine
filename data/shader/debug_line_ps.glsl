#version 330

in VS_OUT
{
    smooth vec4 Color;
} fs_in;

out vec4 OutColor;

void main()
{
    OutColor = fs_in.Color;
}