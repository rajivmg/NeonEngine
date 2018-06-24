#ifndef NEON_GAME_H
#define NEON_GAME_H

#include <vector>
#include <core/neon_bitmap.h>
#include <core/neon_text.h>
#include "neon_editor.h"

#define MAX_TILEMAP_X 200
#define MAX_TILEMAP_Y 200
#define MAX_TILEMAP_LAYERS 10

struct tileset
{
    char *File;
    bitmap *Bitmap;
};
struct tile
{
    u16 ID;     // Tile ID
    u16 TsID;   // Tileset ID
    r32 UV0, UV1, UV2, UV3; // BLx, BLy, TRx, TRy
};
struct tilemap
{
    std::vector<tileset> Tilesets;
    std::vector<tile> Tiles;
    u16 Map[MAX_TILEMAP_Y][MAX_TILEMAP_X];
    u32 MapSizeX;
    u32 MapSizeY;
};

struct game_state
{
    render_resource WhiteTexture;

    render_cmd_list *WorldRender;
    render_resource WorldVertexBuffer;
    std::vector<vert_P1C1UV1> WorldVertices;

    render_resource SpriteShader;

    font DbgFont;

    render_resource TextShader;

    render_resource DbgTextVertexBuffer;
    render_cmd_list *DbgTextRender;

    render_resource DbgLineShader;
    render_resource DbgLineVertexBuffer;
    std::vector<vert_P1C1> DbgLineVertices;
    render_cmd_list *DbgLineRender;

    r32 MetersToPixels;
    r32 PixelsToMeters;

    render_resource EditorTilesetTexture;
    render_resource GameTilesetTexture;

    editor_state EditorState;
};
#endif