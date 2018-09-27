#include "assetsystem.h"

#include <rapidxml/rapidxml.hpp>
using namespace rapidxml;
#include <string.h>

void asset_manager::Init()
{
    TileCapacity = 512;
    Tiles = (game_tile *)MALLOC(TileCapacity * sizeof(game_tile));

    // NOTE: Load Assets
    file_content AssetsXMLFile = Platform.ReadFile("Assets.xml");
    ASSERT(AssetsXMLFile.NoError);

    // NOTE: Copy file content into a null-terminated string
    char *NullTerminatedFile = (char *)MALLOC((AssetsXMLFile.Size + 1) * sizeof(char));
    memcpy(NullTerminatedFile, AssetsXMLFile.Content, AssetsXMLFile.Size + 1);
    NullTerminatedFile[AssetsXMLFile.Size] = '\0';

    // NOTE: Free FileContent
    Platform.FreeFileContent(&AssetsXMLFile);

    // NOTE: Parse XML doc
    xml_document<> Doc;
    Doc.parse<0>(NullTerminatedFile);

    // NOTE: Load Tiledata
    xml_node<> *TileDataNode = Doc.first_node("tiledata");
    while(TileDataNode)
    {
        xml_attribute<> *TileDataBitmapAttr = TileDataNode->first_attribute("bitmap");
        xml_attribute<> *TileDataBitmapWAttr = TileDataNode->first_attribute("bitmapwidth");
        xml_attribute<> *TileDataBitmapHAttr = TileDataNode->first_attribute("bitmapheight");
        xml_attribute<> *TileDataCountAttr = TileDataNode->first_attribute("count");

        u32 BitmapWidth = strtoul(TileDataBitmapWAttr->value(), nullptr, 10);
        u32 BitmapHeight = strtoul(TileDataBitmapHAttr->value(), nullptr, 10);
        TileCount = strtoul(TileDataCountAttr->value(), nullptr, 10);
        ASSERT(TileCount <= TileCapacity);
        xml_node<> *TileNode = TileDataNode->first_node("tile");
        u32 TileSize = 16, TileIndex = 0;
        while(TileNode)
        {
            xml_attribute<> *TileIdAttr = TileNode->first_attribute("id");
            xml_attribute<> *TileXAttr = TileNode->first_attribute("x");
            xml_attribute<> *TileYAttr = TileNode->first_attribute("y");

            u32 X = strtoul(TileXAttr->value(), nullptr, 10);
            u32 Y = strtoul(TileYAttr->value(), nullptr, 10);

            game_tile *Tile = Tiles + TileIndex;
            Tile->ID = (u16)strtoul(TileIdAttr->value(), nullptr, 10);
            Tile->UV.x = X / (r32)BitmapWidth;
            Tile->UV.y = (BitmapHeight - Y - TileSize) / (r32)BitmapHeight;
            Tile->UV.z = (X + TileSize) / (r32)BitmapWidth;
            Tile->UV.w = (BitmapHeight - Y) / (r32)BitmapHeight;

            ++TileIndex;
            TileNode = TileNode->next_sibling("tile");
        }
        TileDataNode = TileDataNode->next_sibling("tiledata");
    }

    // NOTE: Free Doc and NullTerminatedFile
    Doc.clear();
    SAFE_FREE(NullTerminatedFile);
}

void asset_manager::Shutdown()
{
    TileCapacity = 0;
    TileCount = 0;
    SAFE_FREE(Tiles);
}

game_tile *asset_manager::GetTileByID(u16 ID)
{
    // NOTE: Binary search the Tiles array
    u32 Lo = 0, Hi = TileCount - 1, Mid;
    while(Lo <= Hi)
    {
        Mid = (Lo + Hi) / 2;
        if(Tiles[Mid].ID < ID)
        {
            Lo = Mid + 1;
        }
        else if(Tiles[Mid].ID > ID)
        {
            Hi = Mid - 1;
        }
        else
        {
            return &Tiles[Mid];
        }
    }
    ASSERT(!"Tile not found!");
    return nullptr;
}

