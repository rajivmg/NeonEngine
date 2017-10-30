#ifndef NEON_SDL_H
#define NEON_SDL_H

#include "neon_platform.h"

struct game_code
{
	void *Handle;
	game_update_and_render 	*GameUpdateAndRender;
	game_code_loaded 		*GameCodeLoaded;
	imgui_render_draw_lists *ImGui_RenderDrawLists;
	imgui_create_device_objects *ImGui_CreateDeviceObjects;
	imgui_invalidate_device_objects *ImGui_InvalidateDeviceObjects;
};

#endif
