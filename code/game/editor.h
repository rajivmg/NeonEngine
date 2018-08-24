#ifndef EDITOR_H
#define EDITOR_H

#include <core/neon_platform.h>
#include <core/neon_renderer.h>

struct editor_tile
{
    u32 ID;
    u32 X, Y, W, H;
    char Name[128];
};

struct editor_game_tile
{
    u16 ID;
    bool Collide;
};

enum editor_edit_mode
{
    EditMode_Brush,
    EditMode_Erase
};

struct editor_state
{
    editor_tile *EditorTiles;
    editor_tile *EditorGrid[1024][1024];
    editor_tile *BrushTile;
    editor_edit_mode EditMode;

    r32 AtlasWidth, AtlasHeight;
    render_resource AtlasTexture, SRGBAtlasTexture;

    bool WindowHovered;

    s32 LevelWidth, LevelHeight;
    editor_game_tile *EditLevel;

    // Internal
    char *TilesetXmlFile;
    r32 MetersToPixels;
    r32 PixelsToMeters;
    vec3 CameraP;
    mat4 ViewMatrix;
    mat4 ProjMatrix;

    render_cmd_list *DbgLineRender;
    render_resource DbgLineShader;
    render_resource DbgLineVertexBuffer;
    std::vector<vert_P1C1> DbgLineVertices;

    render_cmd_list *SpriteRender;
    render_resource SpriteShader;
    render_resource SpriteVertexBuffer;
    std::vector<vert_P1C1UV1> SpriteVertices;
};

void InitEditor(editor_state *EditorState);
void EditorUpdateAndRender(editor_state *EditorState, game_input *GameInput);
#endif