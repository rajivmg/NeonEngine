#ifndef F_EDITOR_H
#define F_EDITOR_H

#include <core/neon_platform.h>
#include <core/neon_renderer.h>

struct editor_tile
{
    u32 ID;
    u32 X, Y, W, H;
    char Name[128];
};

struct editor_state
{
    editor_tile *EditorTiles;
    editor_tile *EditorGrid[1024][1024];

    render_resource AtlasTexture;

    editor_tile *SelectedTile;
    // Internal
    char *TilesetXmlFile;
};

void InitEditor(editor_state *State);
void EditorUpdate(editor_state *State);
#endif