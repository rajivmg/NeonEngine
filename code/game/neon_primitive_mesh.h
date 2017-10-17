#ifndef NEON_PRIMITIVE_MESH
#define NEON_PRIMITIVE_MESH

struct texture_quad
{
	// 6 vertices
	// 1 vertex data = Pos(3 floats) + Color(4 floats) + UV(2 floats) = 9 floats
	r32 Content[54];
};
struct color_quad
{
	// 6 vertices
	// 1 vertex data = Pos(3 floats) + Color(4 floats) = 7 floats
	r32 Content[42];
};
struct line
{
	// 2 vertices
	// 1 Vertex data = Pos(3 floats) + Color (4 floats) = 7 floats
	r32 Content[14];
};

void TextureQuad(texture_quad *Quad, vec3 Origin, vec2 Size, vec4 UVCoords, vec4 Color);
void ColorQuad(color_quad *Quad, vec3 Origin, vec2 Size, vec4 Color);
void Line(line *Line, vec3 Start, vec3 End, vec4 Color);

#endif