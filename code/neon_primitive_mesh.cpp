#include "neon_primitive_mesh.h"

void TextureQuad(texture_quad *Quad, vec3 Origin, vec2 Size, vec4 UVCoords, vec4 Color)
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

	B.X   = Origin.X + Size.x - 1
	B.Y   = Origin.Y
	B.Z   = 0
	B.U   = UVCoords.z 
	B.V   = UVCoords.y 

	C.X   = Origin.X + Size.x - 1
	C.Y   = Origin.Y + Size.y -1
	C.Z   = 0
	C.UV  = UVCoords.zW

	D.X   = Origin.X 
	D.Y   = Origin.Y + Size.y - 1
	D.Z   = 0
	D.U   = UVCoords.x
	D.V   = UVCoords.w
 */ 
 	// Upper triangle
	// D
	Quad->Content[0] = Origin.x;
	Quad->Content[1] = Origin.y + Size.y;
	Quad->Content[2] = Origin.z;

	Quad->Content[3]  = Color.r;
	Quad->Content[4]  = Color.g;
	Quad->Content[5]  = Color.b;
	Quad->Content[6]  = Color.a;

	Quad->Content[7]  = UVCoords.x;
	Quad->Content[8]  = UVCoords.w;

	// A
	Quad->Content[9] = Origin.x;
	Quad->Content[10] = Origin.y;
	Quad->Content[11] = Origin.z;

	Quad->Content[12] = Color.r;
	Quad->Content[13] = Color.g;
	Quad->Content[14] = Color.b;
	Quad->Content[15] = Color.a;
  
	Quad->Content[16] = UVCoords.x;
	Quad->Content[17] = UVCoords.y;

	// C
	Quad->Content[18]  = Origin.x + Size.x;
	Quad->Content[19] = Origin.y + Size.y;
	Quad->Content[20] = Origin.z;

	Quad->Content[21] = Color.r;
	Quad->Content[22] = Color.g;
	Quad->Content[23] = Color.b;
	Quad->Content[24] = Color.a;

	Quad->Content[25] = UVCoords.z;
	Quad->Content[26] = UVCoords.w;

	// Lower triangle
	// C
	Quad->Content[27] = Origin.x + Size.x;
	Quad->Content[28] = Origin.y + Size.y;
	Quad->Content[29] = Origin.z;

	Quad->Content[30] = Color.r;
	Quad->Content[31] = Color.g;
	Quad->Content[32] = Color.b;
	Quad->Content[33] = Color.a;

	Quad->Content[34] = UVCoords.z;
	Quad->Content[35] = UVCoords.w;

	// A
	Quad->Content[36] = Origin.x;
	Quad->Content[37] = Origin.y;
	Quad->Content[38] = Origin.z;

	Quad->Content[39] = Color.r;
	Quad->Content[40] = Color.g;
	Quad->Content[41] = Color.b;
	Quad->Content[42] = Color.a;
  
	Quad->Content[43] = UVCoords.x;
	Quad->Content[44] = UVCoords.y;

	// B
	Quad->Content[45] = Origin.x + Size.x;
	Quad->Content[46] = Origin.y;
	Quad->Content[47] = Origin.z;

	Quad->Content[48] = Color.r;
	Quad->Content[49] = Color.g;
	Quad->Content[50] = Color.b;
	Quad->Content[51] = Color.a;

	Quad->Content[52] = UVCoords.z;
	Quad->Content[53] = UVCoords.y;
}

void ColorQuad(color_quad *Quad, vec3 Origin, vec2 Size, vec4 Color)
{
	// Upper triangle
	// D
	Quad->Content[0] = Origin.x;
	Quad->Content[1] = Origin.y + Size.y;
	Quad->Content[2] = Origin.z;

	Quad->Content[3]  = Color.r;
	Quad->Content[4]  = Color.g;
	Quad->Content[5]  = Color.b;
	Quad->Content[6]  = Color.a;

	// A
	Quad->Content[7] = Origin.x;
	Quad->Content[8] = Origin.y;
	Quad->Content[9] = Origin.z;

	Quad->Content[10] = Color.r;
	Quad->Content[11] = Color.g;
	Quad->Content[12] = Color.b;
	Quad->Content[13] = Color.a;

	// C
	Quad->Content[14]  = Origin.x + Size.x;
	Quad->Content[15] = Origin.y + Size.y;
	Quad->Content[16] = Origin.z;

	Quad->Content[17] = Color.r;
	Quad->Content[18] = Color.g;
	Quad->Content[19] = Color.b;
	Quad->Content[20] = Color.a;

	// Lower triangle
	// C
	Quad->Content[21] = Origin.x + Size.x;
	Quad->Content[22] = Origin.y + Size.y;
	Quad->Content[23] = Origin.z;

	Quad->Content[24] = Color.r;
	Quad->Content[25] = Color.g;
	Quad->Content[26] = Color.b;
	Quad->Content[27] = Color.a;

	// A
	Quad->Content[28] = Origin.x;
	Quad->Content[29] = Origin.y;
	Quad->Content[30] = Origin.z;

	Quad->Content[31] = Color.r;
	Quad->Content[32] = Color.g;
	Quad->Content[33] = Color.b;
	Quad->Content[34] = Color.a;

	// B
	Quad->Content[35] = Origin.x + Size.x;
	Quad->Content[36] = Origin.y;
	Quad->Content[37] = Origin.z;

	Quad->Content[38] = Color.r;
	Quad->Content[39] = Color.g;
	Quad->Content[40] = Color.b;
	Quad->Content[41] = Color.a;
}

void Line(line *Line, vec3 Start, vec3 End, vec4 Color)
{
	Line->Content[0] = Start.x;
	Line->Content[1] = Start.y;
	Line->Content[2] = Start.z;
	
	Line->Content[3] = Color.r;
	Line->Content[4] = Color.g;
	Line->Content[5] = Color.b;
	Line->Content[6] = Color.a;

	Line->Content[7] = End.x;
	Line->Content[8] = End.y;
	Line->Content[9] = End.z;

	Line->Content[10] = Color.r;
	Line->Content[11] = Color.g;
	Line->Content[12] = Color.b;
	Line->Content[13] = Color.a;
}