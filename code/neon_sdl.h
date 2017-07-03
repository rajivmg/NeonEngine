#ifndef NEON_SDL_H
#define NEON_SDL_H

#include "neon_platform.h"

struct game_code
{
	void *Handle;
	game_update_and_render 	*GameUpdateAndRender;
	game_code_loaded 		*GameCodeLoaded;
};

#endif
