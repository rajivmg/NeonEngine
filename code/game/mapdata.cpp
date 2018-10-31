#include "mapdata.h"

#include <core/neon_bitmap.h>

#include "gamestate.h"

#include <string.h>
#include <rapidxml/rapidxml.hpp>
using namespace rapidxml;

void map_data::Init(const char *Filename)
{
    file_content MapFile = Platform.ReadFile(Filename);
    ASSERT(MapFile.NoError);

    char *NullTermMapFile = (char *)MALLOC((MapFile.Size + 1) * sizeof(char));
    memcpy(NullTermMapFile, MapFile.Content, MapFile.Size);
    NullTermMapFile[MapFile.Size] = '\0';

    Platform.FreeFileContent(&MapFile);

    xml_document<> MapDoc;
    MapDoc.parse<0>(NullTermMapFile);
    xml_node<> *MapNode = MapDoc.first_node("map");
    ASSERT(MapNode);
    xml_attribute<> *MapWidth = MapNode->first_attribute("width");
    xml_attribute<> *MapHeight = MapNode->first_attribute("height");
    xml_attribute<> *MapTileWidth = MapNode->first_attribute("tilewidth");
    xml_attribute<> *MapTileHeight = MapNode->first_attribute("tileheight");

    Width = strtoul(MapWidth->value(), nullptr, 10);
    Height = strtoul(MapHeight->value(), nullptr, 10);
    TileWidth = strtoul(MapTileWidth->value(), nullptr, 10);
    TileHeight = strtoul(MapTileHeight->value(), nullptr, 10);

    // NOTE: Read tilesets
    TilesetCount = 0;
    xml_node<> *TilesetNode = MapNode->first_node("tileset");
    while(TilesetNode)
    {
        ASSERT(TilesetCount < ARRAY_COUNT(map_data::Tilesets));
        
        Tilesets[TilesetCount] = (tileset *)MALLOC(sizeof(tileset));
        tileset *Tileset = Tilesets[TilesetCount];

        xml_attribute<> *TilesetFirstGID = TilesetNode->first_attribute("firstgid");
        xml_attribute<> *TilesetSource = TilesetNode->first_attribute("source");
        Tileset->FirstGID = strtoul(TilesetFirstGID->value(), nullptr, 10);

        // NOTE: Parse tsx file
        char *TilesetFilename = TilesetSource->value();
        while(*TilesetFilename != '/') { ++TilesetFilename; } ++TilesetFilename;
        file_content TSXFile = Platform.ReadFile(TilesetFilename);

        ASSERT(TSXFile.NoError);
        char *NullTermTSXFile = (char *)MALLOC((TSXFile.Size + 1) * sizeof(char));
        memcpy(NullTermTSXFile, TSXFile.Content, TSXFile.Size);
        NullTermTSXFile[TSXFile.Size] = '\0';
        Platform.FreeFileContent(&TSXFile);

        xml_document<> TSXDoc;
        TSXDoc.parse<0>(NullTermTSXFile);

        xml_node<> *TSXTilesetNode = TSXDoc.first_node("tileset");
        xml_attribute<> *TilesetTileWidth = TSXTilesetNode->first_attribute("tilewidth");
        xml_attribute<> *TilesetTileHeight = TSXTilesetNode->first_attribute("tileheight");
        xml_attribute<> *TilesetTileCount = TSXTilesetNode->first_attribute("tilecount");
        xml_attribute<> *TilesetColumns = TSXTilesetNode->first_attribute("columns");

        Tileset->TileWidth = strtoul(TilesetTileWidth->value(), nullptr, 10);
        Tileset->TileHeight = strtoul(TilesetTileHeight->value(), nullptr, 10);
        Tileset->TileCount = strtoul(TilesetTileCount->value(), nullptr, 10);
        Tileset->Columns = strtoul(TilesetColumns->value(), nullptr, 10);

        xml_node<> *TilesetImageNode = TSXTilesetNode->first_node("image");
        xml_attribute<> *TilesetImageSource = TilesetImageNode->first_attribute("source");

        char TilesetBitmapFilename[512];
        strcpy(TilesetBitmapFilename, "tileset/");
        strncat(TilesetBitmapFilename, TilesetImageSource->value(), TilesetImageSource->value_size());
       
        strncpy(Tileset->BitmapFilename, TilesetBitmapFilename, 128); Tileset->BitmapFilename[127] = '\0';
        
        u32 TileIndex = 0;
        u32 TilesetWidth = TileWidth * Tileset->Columns;
        u32 TilesetHeight = TileHeight * (Tileset->TileCount / Tileset->Columns);
        Tileset->Tiles = (tile *)MALLOC(Tileset->TileCount * sizeof(tile));

        // NOTE: Generate ID and UV for all tiles in the tileset image
        for(u32 Row = 0; Row < (Tileset->TileCount / Tileset->Columns); ++Row)
        {
            for(u32 Col = 0; Col < Tileset->Columns; ++Col)
            {
                tile *Tile = &Tileset->Tiles[TileIndex];
                Tile->ID = TileIndex;
                vec4 UV;
                UV.x = (Col * Tileset->TileWidth) / (r32)TilesetWidth;
                UV.y = 1.0f - (((Row * Tileset->TileHeight) + Tileset->TileHeight) / (r32)TilesetHeight);
                UV.z = ((Col * Tileset->TileWidth) + Tileset->TileWidth) / (r32)TilesetWidth;
                UV.w = 1.0f - ((Row * Tileset->TileHeight) / (r32)TilesetHeight);
                Tile->UV[0] = UV;
                Tile->FrameCount = 1;
                Tile->FrameDuration = 0;
                ++TileIndex;
            }
        }

        // NOTE: Process tiles with properties or animation
        Tileset->AnimatedTileCount = 0;
        xml_node<> *TSXTileNode = TSXTilesetNode->first_node("tile");
        while(TSXTileNode)
        {
            // TODO: If these varibles are not used by anything other than if(TileAnimNode), then move them inside that branch
            xml_attribute<> *TileID = TSXTileNode->first_attribute("id");
            u32 Index = strtoul(TileID->value(), nullptr, 10);
            tile *Tile = &Tileset->Tiles[Index];
            //ASSERT(Tile->ID == Index);

            xml_node<> *TileAnimNode = TSXTileNode->first_node("animation");
            if(TileAnimNode)
            {
                ++Tileset->AnimatedTileCount;
                Tile->FrameCount = 0;
                xml_node<> *TileAnimFrameNode = TileAnimNode->first_node("frame");
                while(TileAnimFrameNode)
                {
                    xml_attribute<> *TileAnimFrameTileID = TileAnimFrameNode->first_attribute("tileid");
                    xml_attribute<> *TileAnimFrameDuration = TileAnimFrameNode->first_attribute("duration");

                    u32 FrameTileIndex = strtoul(TileAnimFrameTileID->value(), nullptr, 10);
                    u32 FrameDuration = strtoul(TileAnimFrameDuration->value(), nullptr, 10);

                    Tile->UV[Tile->FrameCount] = Tileset->Tiles[FrameTileIndex].UV[0];

                    Tile->FrameDuration = FrameDuration;
                    ++Tile->FrameCount;
                    TileAnimFrameNode = TileAnimFrameNode->next_sibling("frame");
                }
            }
            // TODO: Read properties here

            TSXTileNode = TSXTileNode->next_sibling("tile");
        }

        TSXDoc.clear();
        SAFE_FREE(NullTermTSXFile);

        ++TilesetCount;
        TilesetNode = TilesetNode->next_sibling("tileset");
    }

    // NOTE: Read tile layers
    TileLayerCount = 0;
    xml_node<> *LayerNode = MapNode->first_node("layer");
    while(LayerNode)
    {
        ASSERT(TileLayerCount < ARRAY_COUNT(map_data::TileLayers));
       
        TileLayers[TileLayerCount] = (tile_layer *)MALLOC(sizeof(tile_layer));
        tile_layer *Layer = TileLayers[TileLayerCount];
        
        xml_attribute<> *LayerID = LayerNode->first_attribute("id");
        xml_attribute<> *LayerName = LayerNode->first_attribute("name");
        Layer->ID = strtoul(LayerID->value(), nullptr, 10);
        strncpy(Layer->Name, LayerName->value(), 64); Layer->Name[63] = '\0';

        // NOTE: Check tile layer collision property
        if(!strncmp("[C]", Layer->Name, 3))
        {
            Layer->Collide = true;
        }
        else
        {
            Layer->Collide = false;
        }

        Layer->Tiles = (u32 *)MALLOC(Width * Height * sizeof(u32));

        xml_node<> *DataNode = LayerNode->first_node("data");
        u32 TileIndex = 1;
        xml_node<> *TileNode = DataNode->first_node("tile");

        for(s32 Y = (s32)Height - 1; Y >= 0; --Y)
        {
            for(s32 X = 0; X < (s32)Width; ++X)
            {
                xml_attribute<> *TileGID = TileNode->first_attribute("gid");
                if(TileGID)
                {
                    Layer->Tiles[X + (Y * Width)] = strtoul(TileGID->value(), nullptr, 10);
                }
                else
                {
                    Layer->Tiles[X + (Y * Width)] = 0;
                }
                TileNode = TileNode->next_sibling("tile");
            }
        }

        ++TileLayerCount;
        LayerNode = LayerNode->next_sibling("layer");
    }

    // NOTE: Read object layers
    ObjectLayerCount = 0;
    xml_node<> *ObjectLayerNode = MapNode->first_node("objectgroup");
    while(ObjectLayerNode)
    {
        ASSERT(ObjectLayerCount < ARRAY_COUNT(ObjectLayers));

        ObjectLayers[ObjectLayerCount] = (object_layer *)MALLOC(sizeof(object_layer));

        object_layer *ObjectLayer = ObjectLayers[ObjectLayerCount];

        xml_attribute<> *LayerID = ObjectLayerNode->first_attribute("id");
        xml_attribute<> *LayerName = ObjectLayerNode->first_attribute("name");
        ObjectLayer->ID = strtoul(LayerID->value(), nullptr, 10);
        strncpy(ObjectLayer->Name, LayerName->value(), 64); ObjectLayer->Name[63] = '\0';

        // NOTE: Count objects in objectgroup
        ObjectLayer->ObjectCount = 0;
        xml_node<> *ObjectCountNode = ObjectLayerNode->first_node("object");
        while(ObjectCountNode)
        {
            ++ObjectLayer->ObjectCount;
            ObjectCountNode = ObjectCountNode->next_sibling("object");
        }

        // NOTE: Read object data
        ObjectLayer->Objects = (object *)MALLOC(ObjectLayer->ObjectCount * sizeof(object));

        u32 ObjectIndex = 0;
        xml_node<> *ObjectNode = ObjectLayerNode->first_node("object");
        while(ObjectNode)
        {
            xml_attribute<> *ObjectID = ObjectNode->first_attribute("id");
            xml_attribute<> *ObjectName = ObjectNode->first_attribute("name");
            xml_attribute<> *ObjectX = ObjectNode->first_attribute("x");
            xml_attribute<> *ObjectY = ObjectNode->first_attribute("y");
            xml_attribute<> *ObjectWidth = ObjectNode->first_attribute("width");
            xml_attribute<> *ObjectHeight = ObjectNode->first_attribute("height");
            xml_node<> *ObjectPointNode = ObjectNode->first_node("point");

            object *Object = &ObjectLayer->Objects[ObjectIndex];
            Object->ID = strtoul(ObjectID->value(), nullptr, 10);
            strncpy(Object->Name, ObjectName->value(), 64); Object->Name[63] = '\0';
            if(ObjectPointNode)
            {
                Object->Point = true;
                Object->Rectangle = false;
                r32 X = strtof(ObjectX->value(), nullptr);
                r32 Y = strtof(ObjectY->value(), nullptr);
                //Object->x = X;
                //Object->y = (Height * TileHeight) - Y; // NOTE: Convert from top-left to bottom-left origin
                // TODO: Simplify this
                Object->x = X / TileWidth; //(X / (Width * TileWidth)) * Width;
                Object->y = ((Height * TileHeight) - Y) / TileHeight; //(((Height * TileHeight) - Y) / (Height * TileHeight)) * Height; // NOTE: Convert from top-left to bottom-left origin
            }
            else
            {
                Object->Rectangle = true;
                Object->Point = false;
                r32 X = strtof(ObjectX->value(), nullptr);
                r32 Y = strtof(ObjectY->value(), nullptr);
                r32 W = strtof(ObjectWidth->value(), nullptr);
                r32 H = strtof(ObjectHeight->value(), nullptr);
                //Object->Rect.x = X;
                //Object->Rect.y =  (Height * TileHeight) - (Y + H); // NOTE: Convert from top-left to bottom-left origin
                //Object->Rect.width = W;
                //Object->Rect.height = H;
                Object->Rect.x = X / TileWidth;
                Object->Rect.y = ((Height * TileHeight) - (Y + H)) / TileHeight; // NOTE: Convert from top-left to bottom-left origin
                Object->Rect.width = W / TileWidth;
                Object->Rect.height = H / TileHeight;
            }

            ++ObjectIndex;
            ObjectNode = ObjectNode->next_sibling("object");
        }

        ++ObjectLayerCount;
        ObjectLayerNode = ObjectLayerNode->next_sibling("objectgroup");
    }

    MapDoc.clear();
    SAFE_FREE(NullTermMapFile);
}

void map_data::Shutdown()
{
    for(u32 TilesetIndex = 0; TilesetIndex < TilesetCount; ++TilesetIndex)
    {
        SAFE_FREE(Tilesets[TilesetIndex]->Tiles);
        SAFE_FREE(Tilesets[TilesetIndex]);
    }

    for(u32 TileLayerIndex = 0; TileLayerIndex < TileLayerCount; ++TileLayerIndex)
    {
        SAFE_FREE(TileLayers[TileLayerIndex]->Tiles);
        SAFE_FREE(TileLayers[TileLayerIndex]);
    }

    for(u32 ObjectLayerIndex = 0; ObjectLayerIndex < ObjectLayerCount; ++ObjectLayerIndex)
    {
        SAFE_FREE(ObjectLayers[ObjectLayerIndex]->Objects);
        SAFE_FREE(ObjectLayers[ObjectLayerIndex]);
    }
}

/*
tileset *map_data::GetTilesetByGID(u32 GID)
{
    for(u32 Index = 0; Index < TilesetCount; ++Index)
    {
        tileset *CurrTileset = Tilesets[Index];
        if(CurrTileset->FirstGID <= GID)
        {
            if((Index + 1) >= TilesetCount)
            {
                return CurrTileset;
            }

            tileset *NextTileset = Tilesets[Index + 1];
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
}

object_layer *map_data::GetObjectLayerByName(const char *Name)
{
    for(u32 Index = 0; Index < ObjectLayerCount; ++Index)
    {
        if(!strcmp(ObjectLayers[Index]->Name, Name))
        {
            return ObjectLayers[Index];
        }
    }
    return nullptr;
}

object *map_data::GetObjectByName(object_layer *ObjectLayer, const char *Name)
{
    for(u32 Index = 0; Index < ObjectLayer->ObjectCount; ++Index)
    {
        if(!strcmp(ObjectLayer->Objects[Index].Name, Name))
        {
            return &ObjectLayer->Objects[Index];
        }
    }
    return nullptr;
}

void map_data::PrepareStaticBuffers()
{
    for(u32 LayerIndex = 0; LayerIndex < TileLayerCount; ++LayerIndex)
    {
        tile_layer *Layer = TileLayers[LayerIndex];
        for(u32 Y = 0; Y < Height; ++Y)
        {
            for(u32 X = 0; X < Width; ++X)
            {
                u32 TileGID = Layer->Tiles[X + (Y * Width)];
                if(TileGID)
                {
                    tileset *Tileset = GetTilesetByGID(TileGID);
                    tile *Tile = &Tileset->Tiles[TileGID - Tileset->FirstGID];
                    PushSprite(&Tileset->Vertices, Rect((r32)X, (r32)Y, 1.0f, 1.0f), Tile->UV[0], vec4(1.0f), 0.0f, vec2(0.0f), (r32)LayerIndex);
                }
            }
        }
    }
    
    for(u32 TilesetIndex = 0; TilesetIndex < TilesetCount; ++TilesetIndex)
    {
        tileset *Tileset = Tilesets[TilesetIndex];
        
        u32 BufferSize = (u32)Tileset->Vertices.size() * sizeof(vert_P1C1UV1);
        
        Tileset->VertexBuffer = rndr::MakeBuffer(resource_type::VERTEX_BUFFER, BufferSize, false);
        rndr::BufferData(Tileset->VertexBuffer, 0, BufferSize, &Tileset->Vertices.front());
    }
}

void map_data::Render()
{
    for(u32 TilesetIndex = 0; TilesetIndex < TilesetCount; ++TilesetIndex)
    {
        tileset *Tileset = Tilesets[TilesetIndex];

        cmd::draw *DrawCmd = GameState->SpriteRender->AddCommand<cmd::draw>(0);
        DrawCmd->VertexBuffer = Tileset->VertexBuffer;
        DrawCmd->VertexFormat = vert_format::P1C1UV1;
        DrawCmd->StartVertex = 0;
        DrawCmd->VertexCount = (u32)Tileset->Vertices.size();
        DrawCmd->Textures[0] = Tileset->Texture;
    }

    // Clear dynamic vertices of every tileset
}
*/
