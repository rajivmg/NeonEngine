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
    // Texels are in linear gamma because of GL_SRGB_ALPHA texture format,
    // Color is in sRGB gamma
    
    vec4 Texel = texture(Sampler0, fs_in.UV);

    if(Texel.a < 0.2) // 0.4 orginal
    {
        discard;
    }

    vec4 Color = Texel * vec4(pow(fs_in.Color.rgb, vec3(2.2)), fs_in.Color.a);
    
    OutColor = vec4(pow(Color.rgb, vec3(1.0/2.2)), Color.a);
}