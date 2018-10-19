#include "assetsystem.h"

#include <core/neon_bitmap.h>
#include <rapidxml/rapidxml.hpp>

#include "gamestate.h"

#include <string.h>

using namespace rapidxml;

void game_tileset::Init(const char *Filename)
{
    file_content TilesetFile = Platform.ReadFile(Filename);
    ASSERT(TilesetFile.NoError);

    char *NullTermTilesetFile = (char *)MALLOC((TilesetFile.Size + 1) * sizeof(char));
    memcpy(NullTermTilesetFile, TilesetFile.Content, TilesetFile.Size);
    NullTermTilesetFile[TilesetFile.Size] = '\0';
    Platform.FreeFileContent(&TilesetFile);

    // NOTE: Parse null-terminated Tileset file
    xml_document<> TilesetDoc;
    TilesetDoc.parse<0>(NullTermTilesetFile);

    xml_node<> *TilesetNode = TilesetDoc.first_node("tileset");
    xml_attribute<> *TilesetTileWidth = TilesetNode->first_attribute("tilewidth");
    xml_attribute<> *TilesetTileHeight = TilesetNode->first_attribute("tileheight");
    xml_attribute<> *TilesetTileCount = TilesetNode->first_attribute("tilecount");
    xml_attribute<> *TilesetColumns = TilesetNode->first_attribute("columns");

    u32 TileWidth = strtoul(TilesetTileWidth->value(), nullptr, 10);
    u32 TileHeight = strtoul(TilesetTileHeight->value(), nullptr, 10);
    u32 Columns = strtoul(TilesetColumns->value(), nullptr, 10);

    TileCount = strtoul(TilesetTileCount->value(), nullptr, 10);

    xml_node<> *TilesetImageNode = TilesetNode->first_node("image");
    xml_attribute<> *TilesetImageSource = TilesetImageNode->first_attribute("source");

    // NOTE: Extract bitmap's absolute path
    char *TilesetBitmapFilename = TilesetImageSource->value();
    while(*TilesetBitmapFilename != '/') { ++TilesetBitmapFilename; } ++TilesetBitmapFilename;

    // NOTE: Generate texture
    bitmap TilesetBitmap;
    LoadBitmap(&TilesetBitmap, TilesetBitmapFilename);
    Texture = rndr::MakeTexture(&TilesetBitmap, tex_param::TEX2D, tex_param::NEAREST, tex_param::CLAMP, true);
    FreeBitmap(&TilesetBitmap);

    u32 TileIndex = 0;
    u32 TilesetWidth = TileWidth * Columns;
    u32 TilesetHeight = TileHeight * (TileCount / Columns);
    Tiles = (game_tile *)MALLOC(TileCount * sizeof(game_tile));

    // NOTE: Generate ID and UV for all tiles in the tileset image
    for(u32 Row = 0; Row < (TileCount / Columns); ++Row)
    {
        for(u32 Col = 0; Col < Columns; ++Col)
        {
            game_tile *Tile = &Tiles[TileIndex];
            Tile->ID = TileIndex;
            vec4 UV;
            UV.x = (Col * TileWidth) / (r32)TilesetWidth;
            UV.y = 1.0f - (((Row * TileHeight) + TileHeight) / (r32)TilesetHeight);
            UV.z = ((Col * TileWidth) + TileWidth) / (r32)TilesetWidth;
            UV.w = 1.0f - ((Row * TileHeight) / (r32)TilesetHeight);
            Tile->UV[0] = UV;
            Tile->FrameCount = 1;
            Tile->FrameDuration = 0;
            ++TileIndex;
        }
    }

    // NOTE: Process animated tiles in the tileset
    xml_node<> *TilesetTileNode = TilesetNode->first_node("tile");
    while(TilesetTileNode)
    {
        xml_node<> *TileAnimationNode = TilesetTileNode->first_node("animation");
        if(TileAnimationNode)
        {
            xml_attribute<> *TileID = TilesetTileNode->first_attribute("id");
            u32 Index = strtoul(TileID->value(), nullptr, 10);
            game_tile *Tile = &Tiles[Index];
            ASSERT(Tile->ID == Index);

            Tile->FrameCount = 0;
            xml_node<> *AnimFrameNode = TileAnimationNode->first_node("frame");
            while(AnimFrameNode)
            {
                xml_attribute<> *AnimFrameTileID = AnimFrameNode->first_attribute("tileid");
                xml_attribute<> *AnimFrameDuration = AnimFrameNode->first_attribute("duration");

                u32 FrameTileIndex = strtoul(AnimFrameTileID->value(), nullptr, 10);
                u32 FrameDuration = strtoul(AnimFrameDuration->value(), nullptr, 10);

                Tile->UV[Tile->FrameCount] = Tiles[FrameTileIndex].UV[0];
                Tile->FrameDuration = FrameDuration;

                ++Tile->FrameCount;
                AnimFrameNode = AnimFrameNode->next_sibling("frame");
            }
        }
        TilesetTileNode = TilesetTileNode->next_sibling("tile");
    }

    // NOTE: Free resources
    TilesetDoc.clear();
    SAFE_FREE(NullTermTilesetFile);

    // NOTE: Generate vertex buffer
    VertexBuffer = rndr::MakeBuffer(resource_type::VERTEX_BUFFER, MEGABYTE(5), true);
}

void game_tileset::Shutdown()
{
    SAFE_FREE(Tiles);
    rndr::DeleteTexture(Texture);
    rndr::DeleteBuffer(VertexBuffer);
}

game_tile *game_tileset::GetTileByID(u32 ID)
{
    ASSERT(ID < TileCount);
    return &Tiles[ID];
}

void game_tileset::Render()
{
    u32 VertexBufferSize = (u32)Vertices.size() * sizeof(vert_P1C1UV1);
    if(VertexBufferSize > 0)
    {
        rndr::BufferData(VertexBuffer, 0, VertexBufferSize, &Vertices.front());
        Vertices.clear();

        cmd::draw *DrawCmd = GameState->SpriteRender->AddCommand<cmd::draw>(1);
        DrawCmd->VertexBuffer = VertexBuffer;
        DrawCmd->VertexFormat = vert_format::P1C1UV1;
        DrawCmd->StartVertex = 0;
        DrawCmd->VertexCount = VertexBufferSize;
        DrawCmd->Textures[0] = Texture;
    }
}
