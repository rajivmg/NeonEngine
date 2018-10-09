#include "map.h"

#include <core/neon_bitmap.h>

#include "gamestate.h"

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

    //
}

void map::PrepareStaticBuffer()
{
    // NOTE: Returns tileset* that contains the given GID data
    auto GetTilesetByGID = [this](u32 GID) -> map_tileset *
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
                if(NextTileset->FirstGID < GID)
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
    };

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

void map::UpdateDynamicBuffer(game_input *Input)
{
    // NOTE: Returns tileset* that contains the given GID data
    auto GetTilesetByGID = [this](u32 GID) -> map_tileset *
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
                if(NextTileset->FirstGID < GID)
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
    };

    // NOTE: Animate tiles
    for(u32 TilesetIndex = 0; TilesetIndex < TilesetCount; ++TilesetIndex)
    {
        map_tileset *Tileset = &Tilesets[TilesetIndex];

        // NOTE: Animate tiles
        for(u32 AnimTileIndex = 0; AnimTileIndex < Tileset->AnimatedTileCount; ++AnimTileIndex)
        {
            map_tile *Tile = Tileset->AnimatedTiles[AnimTileIndex];
            if((Input->Time - Tile->LastFrameChangeTime) * 1000.0 > (r64)Tile->FrameDuration)
            {
                Tile->LastFrameChangeTime = Input->Time;
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
}

void map::Render()
{
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


