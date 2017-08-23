#ifndef NEON_MESH_H
#define NEON_MESH_H

#include "neon_platform.h"
#include "neon_renderer.h"

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

struct mesh_vertex
{
	r32 Position[3];
	r32 Normal[3];
	r32 TexCoord[2];
	s16 MaterialID; // GLshort
};
struct mesh
{
	mesh_vertex *Vertices;
	u32 VertexCount;
	u32 IndicesCount;
};

enum
{
	COLOR_TEXTURE_MATERIAL_MESH,
	COLOR_TEXTURE_MESH,
	COLOR_MESH,
};



void MeshFromFile(char const * Filename);

#endif