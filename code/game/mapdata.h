#ifndef MAPDATA_H
#define MAPDATA_H

#include <core/neon_platform.h>
#include <core/neon_renderer.h>

struct tile
{
    u32 ID;
    u32 FrameCount;
    u32 FrameDuration;
    vec4 UV[8];
};

struct tileset
{
    u32 FirstGID;
    char BitmapFilename[128];
    u32 TileWidth;
    u32 TileHeight;
    u32 TileCount;
    u32 Columns;
    tile *Tiles;
    u32 AnimatedTileCount;
};

struct tile_layer
{
    u32 ID;
    char Name[64];
    u32 *Tiles;
    bool Collide;
};

struct object
{
    u32 ID;
    char Name[64];
    bool Point;
    bool Rectangle;
    union
    {
        r32 x, y;
        rect Rect;
    };
};

struct object_layer
{
    u32 ID;
    char Name[64];
    object *Objects;
    u32 ObjectCount;
};

struct map_data
{
    u32 Width;
    u32 Height;
    u32 TileWidth;
    u32 TileHeight;

    tileset *Tilesets[8];
    u32 TilesetCount;

    tile_layer *TileLayers[12];
    u32 TileLayerCount;

    object_layer *ObjectLayers[4];
    u32 ObjectLayerCount;

    void Init(const char *Filename);
    void Shutdown();
};

#endif