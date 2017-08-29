#ifndef NEON_GAME_H
#define NEON_GAME_H

#include "neon_platform.h"

#define NEON_INIT_GL
#define NEON_DEBUG_GL
#include "neon_GL.h"

#include "neon_renderer.h"
#include "neon_math.h"
#include "neon_texture.h"
#include "neon_debug.h"

#include "neon_math.cpp"
#include "neon_renderer.cpp"
#include "neon_texture.cpp"
#include "neon_debug.cpp"
#include "neon_opengl.cpp"
#include "neon_primitive_mesh.cpp"

platform_t *Platform;

struct world_position
{
	ivec2 Tile;
	vec2  TileRel;
};

struct level
{
	ivec2 LevelSize;
	u32 *Tiles;
};	

level* AllocLevel(ivec2 LevelSize);

#endif