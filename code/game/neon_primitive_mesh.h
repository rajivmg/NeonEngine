#ifndef NEON_PRIMITIVE_MESH
#define NEON_PRIMITIVE_MESH

#include "neon_math.h"

#include <vector>

struct vert_POS3UV2COLOR4;

void PushSpriteQuad(std::vector<vert_POS3UV2COLOR4> *Container, vec3 P, vec2 Size, vec4 Color, vec4 UV);

#endif