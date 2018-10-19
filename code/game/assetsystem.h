#ifndef ASSETSYSTEM_H
#define ASSETSYSTEM_H

#include <core/neon_platform.h>
#include <core/neon_math.h>
#include <core/neon_renderer.h>

struct game_tile
{
    u32 ID;
    u32 FrameCount;
    u32 FrameDuration;
    r64 LastFrameChangeTime;
    u32 CurrFrameIndex;
    vec4 UV[16];
};

struct game_tileset
{
    game_tile *Tiles;
    u32 TileCount;

    std::vector<vert_P1C1UV1> Vertices;
    render_resource VertexBuffer, Texture;

    void Init(const char *Filename);
    void Shutdown();

    game_tile *GetTileByID(u32 ID);

    void Render();
};
#endif