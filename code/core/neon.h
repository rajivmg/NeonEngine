#ifndef NEON_SDL_H
#define NEON_SDL_H

struct ImDrawData;

void GameSetup();
void GameUpdateAndRender(game_input *Input);

void ImGui_RenderDrawLists(ImDrawData *draw_data);
bool ImGui_CreateDeviceObjects();
void ImGui_InvalidateDeviceObjects();
#endif
