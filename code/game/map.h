#ifndef MAP_H
#define MAP_H

#include <core/neon_platform.h>
#include <core/neon_math.h>

#include "mapdata.h"

struct map_tile
{
    u32 FrameCount;
    u32 FrameDuration;
    r64 LastFrameChangeTime;
    u32 CurrFrameIndex;
    vec4 UV[8];
};

struct map_tileset
{
    u32 FirstGID;
    map_tile *Tiles;
    map_tile **AnimatedTiles;
    u32 AnimatedTileCount;

    std::vector<vert_P1C1UV1> StaticVertices, DynamicVertices;
    render_resource StaticVB, DynamicVB;
    render_resource Texture;
};

struct map_tile_layer
{
    u32 *GIDs;
};

struct map
{
    u32 Width, Height;
    u32 TileWidth, TileHeight;

    map_tileset *Tilesets;
    u32 TilesetCount;
    map_tile_layer *TileLayers;
    u32 TileLayerCount;
    // tile_collider
    // static_collider { manifold, void(*Callback)(void *Data) }
    // entity_collider { manifold, entity *}
    
    void Init(map_data *MapData);
    void PrepareStaticBuffer();
    void UpdateDynamicBuffer(game_input *Input);
    void Render();
};

#endif