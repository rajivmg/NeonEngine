#ifndef NEON_PRIMITIVE_MESH
#define NEON_PRIMITIVE_MESH

#include "neon_math.h"

#include <vector>

struct vert_P1C1UV1;

void PushSprite(std::vector<vert_P1C1UV1> *Vertices, vec3 P, vec2 Size, vec4 Color, vec4 UV);

#endif