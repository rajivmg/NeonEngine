#ifndef F_EDITOR_H
#define F_EDITOR_H

#include <core/neon_platform.h>
#include <core/neon_renderer.h>

struct game_state;
struct editor_tile
{
    u32 ID;
    u32 X, Y, W, H;
    char Name[128];
};

struct editor_ctx
{
    editor_tile *EditorTiles;
    editor_tile *EditorGrid[1024][1024];

    r32 AtlasWidth, AtlasHeight;
    render_resource AtlasTexture;

    bool Hovered;
    editor_tile *SelectedTile;

    // Internal
    char *TilesetXmlFile;
};

void InitEditor(editor_ctx *EditorCtx);
void EditorUpdate(editor_ctx *EditorCtx);
void EditorRender(editor_ctx *EditorCtx, game_state *GameState);
#endif