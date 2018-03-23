#ifndef NEON_MESH_H
#define NEON_MESH_H

#include <vector>
#include "../platform/neon_platform.h"

typedef u16 vert_index;
struct vert_P1C1UV1;
struct vert_P1N1UV1;
enum class vert_format;

// NOTE: Only .fbx files are supported.
void PushMesh(std::vector<vert_P1C1UV1> *Vertices, std::vector<vert_index> *Indices, char const *File, int MeshIndex);
void PushMesh(std::vector<vert_P1N1UV1> *Vertices, std::vector<vert_index> *Indices, char const *File, int MeshIndex);

#endif