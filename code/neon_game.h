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

platform_t *Platform;

#endif