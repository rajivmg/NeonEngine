#ifndef MAP_H
#define MAP_H

#include <core/neon_platform.h>
#include <core/neon_math.h>

#include "mapdata.h"
#include "item.h"

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

struct map_collision_layer
{
    u32 *Colliders;
};

struct map_object
{
    enum type { NONE, POINT, RECTANGLE };

    u32 ID;
    char Name[64];
    type Type;
    union
    {
        struct
        { 
            r32 x, y;
        };
        rect Rect;
    };
};

struct map_farm
{
    rect Area;
    item *Items;
};

struct map
{
    u32 Width, Height;
    u32 TileWidth, TileHeight;

    map_tileset *Tilesets;
    u32 TilesetCount;
    map_tile_layer *TileLayers;
    u32 TileLayerCount;

    map_collision_layer CollisionLayer;

    map_object *Objects;
    u32 ObjectCount;

    map_farm *Farms;
    u32 FarmCount;

    // tile_collider [done]
    // entity_collider { AABB, manifold, *entity->Collided }
    // static_collider { manifold, void(*Callback)(void *Data) }
    
    void Init(map_data *MapData);
    void Shutdown();

    map_tileset *GetTilesetByGID(u32 GID);
    map_object *GetNextObjectByName(const char *_Name, map_object *_Object);
    // NOTE: Return false when Tile at XY is not in a farmable area or it's already occupied 
    bool CanFarm(u32 X, u32 Y, map_farm **_Farm);
    item *FarmGet(u32 X, u32 Y, map_farm *_Farm);
    item *FarmPut(u32 X, u32 Y, map_farm *_Farm, item::type ItemType);
    void FarmRemove(u32 X, u32 Y);

    void GenerateStaticBuffer();
    void UpdateDynamicBuffer();
    void UpdateAndRender();
};

#endif