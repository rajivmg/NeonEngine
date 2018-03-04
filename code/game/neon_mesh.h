#ifndef NEON_MESH_H
#define NEON_MESH_H

#include <vector>
#include "../platform/neon_platform.h"

typedef u16 vert_index;
struct vert_P1UV1C1;
enum class vert_format;

// NOTE: Only .fbx files are supported.
void PushMesh(std::vector<vert_P1UV1C1> *Vertices, std::vector<vert_index> *Indices, char const *File, int MeshIndex);

#endif