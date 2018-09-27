#ifndef ASSETSYSTEM_H
#define ASSETSYSTEM_H

#include <core/neon_platform.h>
#include <core/neon_math.h>

struct render_resource;

struct game_tile
{
    u16 ID;
    vec4 UV;
};

// TODO: Change name
struct asset_manager
{
    game_tile *Tiles;
    u32 TileCount;
    u32 TileCapacity;

    void Init();
    void Shutdown();

    game_tile *GetTileByID(u16 ID);
    render_resource GetTileAtlasTexture();
};
#endif