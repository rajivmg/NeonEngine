#ifndef NEON_GAME_H
#define NEON_GAME_H

#include "neon_platform.h"

#include "neon_math.cpp"
#include "neon_renderer.cpp"
#include "neon_texture.cpp"

#include "neon_renderer.h"
#include "neon_math.h"
#include "neon_texture.h"

struct game_resources
{
	quad_batch dbgQuadBatch;
	text_batch dbgUITextBatch;
	line_batch dbgLineBatch;
}GameResources;

void InitGameResources();

platform_t *Platform;

#endif