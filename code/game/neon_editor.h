#ifndef NEON_EDITOR_H
#define NEON_EDITOR_H

#include <core/neon_platform.h>

struct game_state;

enum editor_mode
{
    EDITOR_TILESET,
    EDITOR_EDIT
};

struct editor_state
{
    char TsFiles[50][64]; // Tileset file names
    s32 TsFilesCount;
    s32 TsFileMax;
};

void InitEditor(editor_state *Editor);
void EditorTick(game_state *GameState);
#endif