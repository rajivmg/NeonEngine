#version 330

uniform sampler2D Sampler0;

in VS_OUT
{
    smooth vec2 UV;
    smooth vec4 Color;
} fs_in;

out vec4 OutColor;

void main()
{
    vec4 Color = texture(Sampler0, fs_in.UV) * vec4(pow(fs_in.Color.rgb, vec3(2.2)), fs_in.Color.a);
    OutColor = vec4(pow(Color.rgb, vec3(1.0/2.2)), Color.a);
    //OutColor = vec4(vec3(texture(Sampler0, fs_in.UV).r), 1.0) * fs_in.Color;
}