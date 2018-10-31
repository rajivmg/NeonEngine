#include "map.h"

#include <core/neon_bitmap.h>

#include "gamestate.h"
#include "debugdraw.h"

void map::Init(map_data *MapData)
{
    Width = MapData->Width; Height = MapData->Height;
    TileWidth = MapData->TileWidth; TileHeight = MapData->TileHeight;

    // NOTE: Prepare tilesets
    TilesetCount = MapData->TilesetCount;
    void *TilesetsAddress = (map_tileset *)GameState->StackAllocator.Allocate(TilesetCount * sizeof(map_tileset), 8);
    Tilesets = new(TilesetsAddress) map_tileset[TilesetCount]();

    for(u32 TilesetIndex = 0; TilesetIndex < TilesetCount; ++TilesetIndex)
    {
        map_tileset *Tileset = &Tilesets[TilesetIndex];
        tileset *MapDataTileset = MapData->Tilesets[TilesetIndex];

        // NOTE: Copy data
        Tileset->FirstGID = MapDataTileset->FirstGID;
        Tileset->Tiles = (map_tile *)GameState->StackAllocator.Allocate(MapDataTileset->TileCount * sizeof(map_tile), 8);
        
        Tileset->AnimatedTileCount = MapDataTileset->AnimatedTileCount;
        Tileset->AnimatedTiles = (map_tile **)GameState->StackAllocator.Allocate(Tileset->AnimatedTileCount * sizeof(map_tile *), 8);
        
        u32 AnimatedTileIndex = 0;
        u32 TileIndex = 0;
        while(TileIndex < MapDataTileset->TileCount)
        {
            map_tile *Tile = &Tileset->Tiles[TileIndex];
            tile *MapDataTile = &MapDataTileset->Tiles[TileIndex];
            Tile->FrameCount = MapDataTile->FrameCount;
            Tile->FrameDuration = MapDataTile->FrameDuration;
            Tile->CurrFrameIndex = 0;
            Tile->LastFrameChangeTime = 0.0;
            if(Tile->FrameCount > 1)
            {
                Tileset->AnimatedTiles[AnimatedTileIndex] = Tile;
                ++AnimatedTileIndex;
            }
            for(u32 FrameIndex = 0; FrameIndex < Tile->FrameCount; ++FrameIndex)
            {
                Tile->UV[FrameIndex] = MapDataTile->UV[FrameIndex];
            }
            ++TileIndex;
        }

        ASSERT(AnimatedTileIndex == Tileset->AnimatedTileCount);

        // NOTE: Generate texture
        bitmap TilesetBitmap;
        LoadBitmap(&TilesetBitmap, MapDataTileset->BitmapFilename);
        Tileset->Texture = rndr::MakeTexture(&TilesetBitmap, tex_param::TEX2D, tex_param::NEAREST, tex_param::CLAMP, true);
        FreeBitmap(&TilesetBitmap);
    }

    // NOTE: Copy tile layers
    TileLayerCount = MapData->TileLayerCount;
    void *TileLayersAddress = (map_tile_layer *)GameState->StackAllocator.Allocate(TileLayerCount * sizeof(map_tile_layer), 8);
    TileLayers = new(TileLayersAddress) map_tile_layer[TileLayerCount]();

    for(u32 TileLayerIndex = 0; TileLayerIndex < TileLayerCount; ++TileLayerIndex)
    {
        map_tile_layer *TileLayer = &TileLayers[TileLayerIndex];
        tile_layer *MapDataTileLayer = MapData->TileLayers[TileLayerIndex];
        TileLayer->GIDs = (u32 *)GameState->StackAllocator.Allocate(Width * Height * sizeof(u32), 8);
        memcpy(TileLayer->GIDs, MapDataTileLayer->Tiles, Width * Height * sizeof(u32));
    }

    // NOTE: Copy objects
    ObjectCount = MapData->ObjectLayers[0]->ObjectCount;
    Objects = (map_object *)MALLOC(ObjectCount * sizeof(map_object));
    for(u32 ObjectIndex = 0; ObjectIndex < ObjectCount; ++ObjectIndex)
    {
        map_object *Object = &Objects[ObjectIndex];
        object *MapDataObject = &MapData->ObjectLayers[0]->Objects[ObjectIndex];
        Object->ID = MapDataObject->ID;
        strncpy(Object->Name, MapDataObject->Name, ARRAY_COUNT(object::Name));
        if(MapDataObject->Point)
        {
            Object->Type = map_object::POINT;
            Object->x = MapDataObject->x; Object->y = MapDataObject->y;
        }
        if(MapDataObject->Rectangle)
        {
            Object->Type = map_object::RECTANGLE;
            Object->Rect = MapDataObject->Rect;
        }
    }

    // NOTE: Generate tile collision map
    CollisionLayer.Colliders = (u32 *)MALLOC(Width * Height * sizeof(u32));
    for(u32 TileLayerIndex = 0; TileLayerIndex < TileLayerCount; ++TileLayerIndex)
    {
        tile_layer *MapDataTileLayer = MapData->TileLayers[TileLayerIndex];
        if(MapDataTileLayer->Collide)
        {
            for(u32 Y = 0; Y < Height; ++Y)
            {
                for(u32 X = 0; X < Width; ++X)
                {
                    CollisionLayer.Colliders[X + (Y * Width)] = MapDataTileLayer->Tiles[X + (Y * Width)] != 0 ? 1 : 0;
                }
            }
        }
    }

    // NOTE: Generate farm map
    FarmCount = 0;
    map_object *FarmableLandObject = GetNextObjectByName("FarmableLand", nullptr);
    while(FarmableLandObject)
    {
        ++FarmCount;
        FarmableLandObject = GetNextObjectByName("FarmableLand", FarmableLandObject);
    }

    FarmableLandObject = GetNextObjectByName("FarmableLand", nullptr);
    Farms = (map_farm *)MALLOC(FarmCount * sizeof(map_farm));
    for(u32 FarmIndex = 0; FarmIndex < FarmCount; ++FarmIndex)
    {
        Farms[FarmIndex].Area = FarmableLandObject->Rect;
        Farms[FarmIndex].Items = (item *)MALLOC((u32)Farms[FarmIndex].Area.width * (u32)Farms[FarmIndex].Area.height * sizeof(item));
        memset(Farms[FarmIndex].Items, 0, (u32)Farms[FarmIndex].Area.width * (u32)Farms[FarmIndex].Area.height * sizeof(item));
        FarmableLandObject = GetNextObjectByName("FarmableLand", FarmableLandObject);
    }
}

void map::Shutdown()
{
}

map_tileset *map::GetTilesetByGID(u32 GID)
{
    for(u32 Index = 0; Index < TilesetCount; ++Index)
    {
        map_tileset *CurrTileset = &Tilesets[Index];
        if(CurrTileset->FirstGID <= GID)
        {
            if((Index + 1) >= TilesetCount)
            {
                return CurrTileset;
            }

            map_tileset *NextTileset = &Tilesets[Index + 1];
            if(NextTileset->FirstGID <= GID)
            {
                continue;
            }
            else
            {
                return CurrTileset;
            }
        }
    }
    INVALID_CODE_PATH;
    return nullptr;
}

map_object *map::GetNextObjectByName(const char *_Name, map_object *_Object)
{
    u32 ObjectIndex = 0;
    
    // NOTE: Move ObjectIndex to next object after _Object
    if(_Object != nullptr)
    {
        bool _ObjectFound = false;
        for(ObjectIndex = 0; ObjectIndex < ObjectCount; ++ObjectIndex)
        {
            if(&Objects[ObjectIndex] == _Object)
            {
                _ObjectFound = true;
                if(ObjectIndex == (ObjectCount - 1))
                {
                    return nullptr; // NOTE: Last object, next object with name _Name not found; return null;
                }

                ObjectIndex++;
                break;
            }
        }

        if(!_ObjectFound)
        {
            INVALID_CODE_PATH;
        }
    }

    // NOTE: Search for next object with _Name
    for(; ObjectIndex < ObjectCount; ++ObjectIndex)
    {
        if(!strncmp(Objects[ObjectIndex].Name, _Name, 64))
        {
            return &Objects[ObjectIndex];
        }
    }

    return nullptr; // NOTE: Next object with name _Name not found; return null
}

bool map::CanFarm(u32 X, u32 Y, map_farm **_Farm)
{
    for(u32 FarmIndex = 0; FarmIndex < FarmCount; ++FarmIndex)
    {
        rect *Area = &Farms[FarmIndex].Area;
        if((Area->x <= X) && (Area->x + Area->width > X)
            && (Area->y <= Y) && (Area->y + Area->height > Y))
        {
            if(_Farm != nullptr)
            {
                *_Farm = &Farms[FarmIndex];
            }
            return true;
        }
    }

    if(_Farm != nullptr)
    {
        *_Farm = nullptr;
    }
    return false;
}

item *map::FarmPut(u32 X, u32 Y, map_farm *_Farm, item::type ItemType)
{
    // NOTE: Coordinates relative to farm position
    u32 RelX = X - (u32)_Farm->Area.x;
    u32 RelY = Y - (u32)_Farm->Area.y;
    
    item *Item = &_Farm->Items[RelX + (RelY * (u32)_Farm->Area.width)];
    Item->Type = ItemType;

    switch(Item->Type)
    {
        case item::FENCE:
        {
            CollisionLayer.Colliders[X + Y * Width] = 1;
        } break;

        INVALID_DEFAULT_CASE;
    }
    return Item;
}

void map::GenerateStaticBuffer()
{
    // NOTE: Generate static vertices for all the tiles in every tile-layers in the map
    for(u32 LayerIndex = 0; LayerIndex < TileLayerCount; ++LayerIndex)
    {
        map_tile_layer *Layer = &TileLayers[LayerIndex];
        for(u32 Y = 0; Y < Height; ++Y)
        {
            for(u32 X = 0; X < Width; ++X)
            {
                u32 TileGID = Layer->GIDs[X + (Y * Width)];
                if(TileGID)
                {
                    map_tileset *Tileset = GetTilesetByGID(TileGID);
                    map_tile *Tile = &Tileset->Tiles[TileGID - Tileset->FirstGID];
                    if(Tile->FrameCount == 1)
                    {
                        PushSprite(&Tileset->StaticVertices, Rect((r32)X, (r32)Y, 1.0f, 1.0f), Tile->UV[0], vec4(1.0f), 0.0f, vec2(0.0f), (r32)LayerIndex);
                    }
                }
            }
        }
    }

    // NOTE: Generate StaticVB for all tile-layers in the map
    for(u32 TilesetIndex = 0; TilesetIndex < TilesetCount; ++TilesetIndex)
    {
        map_tileset *Tileset = &Tilesets[TilesetIndex];
        u32 BufferSize = (u32)Tileset->StaticVertices.size() * sizeof(vert_P1C1UV1);
        if(BufferSize > 0)
        {
            Tileset->StaticVB = rndr::MakeBuffer(resource_type::VERTEX_BUFFER, BufferSize, false);
            rndr::BufferData(Tileset->StaticVB, 0, BufferSize, &Tileset->StaticVertices.front());
        }
    }
}

void map::UpdateDynamicBuffer()
{
    // NOTE: Animate tiles
    for(u32 TilesetIndex = 0; TilesetIndex < TilesetCount; ++TilesetIndex)
    {
        map_tileset *Tileset = &Tilesets[TilesetIndex];

        // NOTE: Animate tiles
        for(u32 AnimTileIndex = 0; AnimTileIndex < Tileset->AnimatedTileCount; ++AnimTileIndex)
        {
            map_tile *Tile = Tileset->AnimatedTiles[AnimTileIndex];
            if((GameState->Time - Tile->LastFrameChangeTime) * 1000.0 >= (r64)Tile->FrameDuration)
            {
                Tile->LastFrameChangeTime = GameState->Time;
                Tile->CurrFrameIndex = (Tile->CurrFrameIndex + 1) % ARRAY_COUNT(map_tile::UV);
            }
        }
    }

    // NOTE: Generate dynamic vertices for all the animated tiles in every tile-layers in the map
    for(u32 LayerIndex = 0; LayerIndex < TileLayerCount; ++LayerIndex)
    {
        map_tile_layer *Layer = &TileLayers[LayerIndex];
        for(u32 Y = 0; Y < Height; ++Y)
        {
            for(u32 X = 0; X < Width; ++X)
            {
                u32 TileGID = Layer->GIDs[X + (Y * Width)];
                if(TileGID)
                {
                    map_tileset *Tileset = GetTilesetByGID(TileGID);
                    map_tile *Tile = &Tileset->Tiles[TileGID - Tileset->FirstGID];
                    if(Tile->FrameCount > 1)
                    {
                        PushSprite(&Tileset->DynamicVertices, Rect((r32)X, (r32)Y, 1.0f, 1.0f), Tile->UV[Tile->CurrFrameIndex], vec4(1.0f), 0.0f, vec2(0.0f), (r32)LayerIndex);
                    }
                }
            }
        }
    }

    // NOTE: Generate DynamicVB for all tile-layers in the map
    for(u32 TilesetIndex = 0; TilesetIndex < TilesetCount; ++TilesetIndex)
    {
        map_tileset *Tileset = &Tilesets[TilesetIndex];

        u32 BufferSize = (u32)Tileset->DynamicVertices.size() * sizeof(vert_P1C1UV1);

        if(Tileset->DynamicVB.Type == resource_type::NOT_INITIALIZED)
        {
            if(BufferSize > 0)
            {
                Tileset->DynamicVB = rndr::MakeBuffer(resource_type::VERTEX_BUFFER, BufferSize, true);
            }
        }
        if(BufferSize > 0)
        {
            rndr::BufferData(Tileset->DynamicVB, 0, BufferSize, &Tileset->DynamicVertices.front());
        }
    }

    // NOTE: Update ItemTileset vertex buffer
    for(u32 FarmIndex = 0; FarmIndex < FarmCount; ++FarmIndex)
    {
        map_farm *Farm = &Farms[FarmIndex];
        for(u32 Y = 0; Y < (u32)Farm->Area.height; ++Y)
        {
            for(u32 X = 0; X < (u32)Farm->Area.width; ++X)
            {
                item *Item = &Farm->Items[X + Y * (u32)Farm->Area.width];
                if(Item->Type == item::FENCE)
                {
                    game_tile *ItemTile = GameState->ItemTileset.GetTileByID(Item->Type);
                    PushSprite(&GameState->ItemTileset.Vertices, Rect((r32)X + Farm->Area.x, (r32)Y + Farm->Area.y, 1.0f, 1.0f), ItemTile->UV[0], vec4(1.0f), 0.0f, vec2(0.0f), 8.0f);
                }
            }
        }
    }

#if 0
    // NOTE: Highlight farmable tiles
    for(u32 FarmIndex = 0; FarmIndex < FarmCount; ++FarmIndex)
    {
        DebugDraw->Rect(Farms[FarmIndex].Area, RGBAUnpackTo01(0x7c34ad55), 7.0f);
    }
#endif
}

void map::UpdateAndRender()
{
    UpdateDynamicBuffer();

    for(u32 TilesetIndex = 0; TilesetIndex < TilesetCount; ++TilesetIndex)
    {
        map_tileset *Tileset = &Tilesets[TilesetIndex];

        // NOTE: Generate draw command per tileset for static tiles
        cmd::draw *StaticTilesDrawCmd = GameState->SpriteRender->AddCommand<cmd::draw>(0);
        StaticTilesDrawCmd->VertexBuffer = Tileset->StaticVB;
        StaticTilesDrawCmd->VertexFormat = vert_format::P1C1UV1;
        StaticTilesDrawCmd->StartVertex = 0;
        StaticTilesDrawCmd->VertexCount = (u32)Tileset->StaticVertices.size();
        StaticTilesDrawCmd->Textures[0] = Tileset->Texture;

        // NOTE: Generate draw command per tileset for dynamic tiles
        cmd::draw *DynamicTilesDrawCmd = GameState->SpriteRender->AddCommand<cmd::draw>(0);
        DynamicTilesDrawCmd->VertexBuffer = Tileset->DynamicVB;
        DynamicTilesDrawCmd->VertexFormat = vert_format::P1C1UV1;
        DynamicTilesDrawCmd->StartVertex = 0;
        DynamicTilesDrawCmd->VertexCount = (u32)Tileset->DynamicVertices.size();
        DynamicTilesDrawCmd->Textures[0] = Tileset->Texture;

        Tileset->DynamicVertices.clear();
    }
}


