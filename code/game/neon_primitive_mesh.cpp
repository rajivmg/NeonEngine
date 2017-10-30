#include "neon_primitive_mesh.h"
#include "neon_renderer.h"

void PushSpriteQuad(std::vector<vert_POS3UV2COLOR4> *Container, vec3 P, vec2 Size, vec4 Color, vec4 UV)
{
	/*
	D--------C
	|  U 	/|
	|      / |
	|  	  /	 |
	|    /   |
	|	/    |
	|  /     |
	| /      |
	|/    B  |
	A--------B

	A.XYZ = Origin.XYZ
	A.UV  = UVCoords.xY

	B.X   = Origin.X + Size.x
	B.Y   = Origin.Y
	B.Z   = 0
	B.U   = UVCoords.z 
	B.V   = UVCoords.y 

	C.X   = Origin.X + Size.x
	C.Y   = Origin.Y + Size.y
	C.Z   = 0
	C.UV  = UVCoords.zW

	D.X   = Origin.X 
	D.Y   = Origin.Y + Size.y
	D.Z   = 0
	D.U   = UVCoords.x
	D.V   = UVCoords.w
	*/ 

	vert_POS3UV2COLOR4 Vertex;

	// D
	Vertex.Position = vec3(P.x, P.y + Size.y, P.z);
	Vertex.UV = vec2(UV.x, UV.w);
	Vertex.Color = Color;
	Container->push_back(Vertex);

	// A
	Vertex.Position = P;
	Vertex.UV = vec2(UV.x, UV.y);
	Vertex.Color = Color;
	Container->push_back(Vertex);

	// C
	Vertex.Position = vec3(P.x + Size.x, P.y + Size.y, P.z);
	Vertex.UV = vec2(UV.z, UV.w);
	Vertex.Color = Color;
	Container->push_back(Vertex);

	// C 
	Vertex.Position = vec3(P.x + Size.x, P.y + Size.y, P.z);
	Vertex.UV = vec2(UV.z, UV.w);
	Vertex.Color = Color;
	Container->push_back(Vertex);

	// A
	Vertex.Position = P;
	Vertex.UV = vec2(UV.x, UV.y);
	Vertex.Color = Color;
	Container->push_back(Vertex);

	// B
	Vertex.Position = vec3(P.x + Size.x, P.y, P.z);
	Vertex.UV = vec2(UV.z, UV.y);
	Vertex.Color = Color;
	Container->push_back(Vertex);
}
