#include "f_editor.h"

#include <core/neon_bitmap.h>
#include <dear-imgui/imgui.h>
#include <rapidxml/rapidxml.hpp>
using namespace rapidxml;

static bool Show_TilesetHelperWindow = false;

void TilesetHelperWindow(editor_state *State)
{
    static bool FileOpened = false;
    static char Filename[256];
    static file_content FileContent;
    static bitmap TilesetBitmap;
    static render_resource TilesetTexture;
    static char *ErrMsg = nullptr;

    ImGui::Begin("Tileset Helper", &Show_TilesetHelperWindow, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_HorizontalScrollbar);

    // Menu bar
    bool MenuFileOpen = false;
    bool MenuFileClose = false;
    if(ImGui::BeginMenuBar())
    {
        if(ImGui::BeginMenu("File"))
        {
            if(ImGui::MenuItem("Open", 0, false, !FileOpened)) { MenuFileOpen = true; }
            if(ImGui::MenuItem("Close", 0, false, FileOpened)) { MenuFileClose = true; };
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    if(MenuFileOpen) { ImGui::OpenPopup("Open File"); }
    if(MenuFileClose) { ImGui::OpenPopup("Close File"); }
    // Menu bar End
#if 0
    ImGui::Text("FILE: %s", Filename);
    if(!FileOpened)
    {
        ImGui::SameLine(ImGui::GetWindowWidth() - 60);
        if(ImGui::Button("OPEN"))
        {
            ImGui::OpenPopup("Open File");
        }
    }
#endif
    if(ImGui::BeginPopupModal("Open File", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::InputText("File", Filename, 128);
        if(ErrMsg)
        {
            ImGui::Text("%s", ErrMsg);
        }
        if(ImGui::Button("OPEN"))
        {
            FileContent = Platform.ReadFile(Filename);
            if(!FileContent.NoError)
            {
                ErrMsg = "Error reading file.";
            }
            else
            {
                LoadBitmap(&TilesetBitmap, FileContent);
                TilesetTexture = rndr::MakeTexture(&TilesetBitmap, tex_param::TEX2D, tex_param::NEAREST, tex_param::CLAMP, false);
                Platform.FreeFileContent(&FileContent);
                FreeBitmap(&TilesetBitmap);
                FileOpened = true;
                ErrMsg = nullptr;
                ImGui::CloseCurrentPopup();
            }
        }
        ImGui::SameLine();
        if(ImGui::Button("CANCEL"))
        {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    if(ImGui::BeginPopupModal("Close File", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Are you sure?");
        if(ImGui::Button("YES"))
        {
            ASSERT(FileOpened);
            rndr::DeleteTexture(TilesetTexture);
            Filename[0] = '\0';
            FileOpened = false;
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if(ImGui::Button("NO"))
        {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
#if 0
    if(FileOpened)
    {
        //ImGui::SameLine(ImGui::GetWindowWidth() - 120);
        //if(ImGui::Button("SAVE"))
        //{

        //}

        ImGui::SameLine(ImGui::GetWindowWidth() - 70);
        if(ImGui::Button("CLOSE"))
        {
            rndr::DeleteTexture(TilesetTexture);
            Filename[0] = '\0';
            FileOpened = false;
        }
    }
#endif

    //ImGui::Separator();

    // Tileset
    if(FileOpened)
    {
        ImGui::Columns(2, "TilesetHelperWindowColumns", true);
        ImGui::BeginChild("CanvasWindow", vec2i(400, 400), true, ImGuiWindowFlags_HorizontalScrollbar);

        static r32 TileScale = 1.0f;
        static vec2 TileSize = vec2(16.0f);
        static vec2 ScaledTileSize = TileSize * TileScale;
        static vec2 ScaledAtlasSize = vec2i(TilesetBitmap.Width, TilesetBitmap.Height) * TileScale;

        static vec2 SelectedTileP = vec2i(-1, -1);
        static ImDrawList *DrawList = ImGui::GetWindowDrawList();
        
        bool IsCanvasHovered;
        vec2 CanvasPos = ImGui::GetCursorScreenPos();

        DrawList->AddRectFilled(CanvasPos, CanvasPos + ScaledAtlasSize, IM_COL32(255, 255, 255, 255));
        DrawList->AddImage(rndr::GetTextureID(TilesetTexture), CanvasPos, CanvasPos + ScaledAtlasSize, ImVec2(0, 1), ImVec2(1, 0));
        ImGui::InvisibleButton("canvas", ScaledAtlasSize);
        IsCanvasHovered = ImGui::IsItemHovered();

        if(IsCanvasHovered)
        {
            if(ImGui::IsMouseClicked(0))
            {
                vec2 MouseP = ImGui::GetIO().MousePos;
                SelectedTileP = vec2(floorf((MouseP.x - CanvasPos.x) / ScaledTileSize.x) * TileSize.x,
                                     floorf((MouseP.y - CanvasPos.y) / ScaledTileSize.y) * TileSize.y);
            }
        }

        for(r32 X = 0; X <= ScaledAtlasSize.x; X += ScaledTileSize.x)
        {
            DrawList->AddLine(CanvasPos + vec2(X, 0), CanvasPos + vec2(X, ScaledAtlasSize.y), 0x7D000000);
        }
        for(r32 Y = 0; Y <= ScaledAtlasSize.y; Y += ScaledTileSize.y)
        {
            DrawList->AddLine(CanvasPos + vec2(0, Y), CanvasPos + vec2(ScaledAtlasSize.x, Y), 0x7D000000);
        }
        ImGui::EndChild();

        ImGui::NextColumn();
        ImGui::BeginChild("HelperWindow", vec2i(400, 400), true);
        ImGui::SameLine(168.0f);
        vec4 SelectedTileUV = vec4(SelectedTileP.x / TilesetBitmap.Width,
                                  (TilesetBitmap.Height - SelectedTileP.y - TileSize.y) / TilesetBitmap.Height,
                                  (SelectedTileP.x + TileSize.x) / TilesetBitmap.Width,
                                  (TilesetBitmap.Height - SelectedTileP.y) / TilesetBitmap.Height);

        ImGui::Image(rndr::GetTextureID(TilesetTexture), ImVec2(64, 64), ImVec2(SelectedTileUV.x, SelectedTileUV.w), ImVec2(SelectedTileUV.z, SelectedTileUV.y));

        ImGui::Spacing();
        static char TileText[768];
        static char TileName[64] = "dummy";
        static s32  TileID = 0;
        ImGui::InputInt("Tile ID", &TileID);
        ImGui::InputText("Tile Name", TileName, 64);
        sprintf(TileText, "<tile id=\"%d\" name=\"%s\" x=\"%d\" y=\"%d\" w=\"%d\" h=\"%d\"/>", TileID, TileName, (s32)SelectedTileP.x, (s32)SelectedTileP.y, (s32)TileSize.x, (s32)TileSize.y);
        //ImGui::Text("x=%.0f, y=%.0f", SelectedTileP.x, SelectedTileP.y);
        ImGui::TextUnformatted(TileText);
        vec2 Size = ImGui::GetItemRectSize();
        ImGui::Spacing();
        if(ImGui::Button("COPY TO CLIPBOARD", vec2(390, 50)))
        {
            ++TileID;
            ImGui::LogToClipboard();
            ImGui::LogText("%s", TileText);
            ImGui::LogFinish();
        }
        ImGui::EndChild();
        ImGui::Columns(1);
    }
    // Tileset end

    ImGui::End();
}

void InitEditor(editor_state *State)
{
    file_content FileData = Platform.ReadFile("Tileset.xml");
    ASSERT(FileData.NoError);

    // Null terminate xml file for parsing
    State->TilesetXmlFile = (char *)MALLOC(sizeof(char) * (FileData.Size + 1));
    memcpy(State->TilesetXmlFile, FileData.Content, FileData.Size + 1);
    State->TilesetXmlFile[FileData.Size] = '\0';

    Platform.FreeFileContent(&FileData);

    // Parse
    xml_document<> Doc;
    Doc.parse<0>(State->TilesetXmlFile);

    xml_node<> *TilesetNode = Doc.first_node("tileset");
    xml_attribute<> *TilesetBitmapAttr = TilesetNode->first_attribute("bitmap");
    xml_attribute<> *TilesetCountAttr = TilesetNode->first_attribute("count");
    
    bitmap AtlasBitmap;
    LoadBitmap(&AtlasBitmap, TilesetBitmapAttr->value());
    State->AtlasTexture = rndr::MakeTexture(&AtlasBitmap, tex_param::TEX2D, tex_param::NEAREST, tex_param::CLAMP, false);
    FreeBitmap(&AtlasBitmap);

    s32 TileCount = strtol(TilesetCountAttr->value(), nullptr, 10);
    State->EditorTiles = (editor_tile *)MALLOC(sizeof(editor_tile) * TileCount);
    Platform.Log("Tile Count: %d", TileCount);
    xml_node<> *TileNode = TilesetNode->first_node("tile");
    s32 TileCounter = 0;
    while(TileNode)
    {
        xml_attribute<> *TileIdAttr = TileNode->first_attribute("id");
        xml_attribute<> *TileNameAttr = TileNode->first_attribute("name");
        xml_attribute<> *TileXAttr = TileNode->first_attribute("x");
        xml_attribute<> *TileYAttr = TileNode->first_attribute("y");
        xml_attribute<> *TileWAttr = TileNode->first_attribute("w");
        xml_attribute<> *TileHAttr = TileNode->first_attribute("h");

        State->EditorTiles[TileCounter].ID = strtoul(TileIdAttr->value(), nullptr, 10);
        strncpy(State->EditorTiles[TileCounter].Name, TileNameAttr->value(), 128);
        State->EditorTiles[TileCounter].X = strtoul(TileXAttr->value(), nullptr, 10);
        State->EditorTiles[TileCounter].Y = strtoul(TileYAttr->value(), nullptr, 10);
        State->EditorTiles[TileCounter].W = strtoul(TileWAttr->value(), nullptr, 10);
        State->EditorTiles[TileCounter].H = strtoul(TileHAttr->value(), nullptr, 10);

        u32 X = State->EditorTiles[TileCounter].X / 16;
        u32 Y = State->EditorTiles[TileCounter].Y / 16;
        State->EditorGrid[X][Y] = &State->EditorTiles[TileCounter];

        ++TileCounter;
        TileNode = TileNode->next_sibling("tile");
    }
    ASSERT(TileCount == TileCounter);
}

void EditorUpdate(editor_state *State)
{
    if(Show_TilesetHelperWindow) TilesetHelperWindow(State);

    static vec4 SelTileUV = vec4i(0, 0, 0, 0);
    ImGui::SetNextWindowSize(ImVec2(350, 560), ImGuiCond_FirstUseEver);
    if(!ImGui::Begin("Editor", 0, ImGuiWindowFlags_MenuBar))
    {
        ImGui::End();
        return;
    }
    if(ImGui::BeginMenuBar())
    {
        if(ImGui::BeginMenu("View"))
        {
            ImGui::MenuItem("Tileset Helper", 0, &Show_TilesetHelperWindow);
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
        //ImGui::Combo("Mode", &EditorMode, "Tileset Creation\0Edit\0");
        ImGui::Spacing();
#if 0
    static int CurrentTileset = 0;
    auto TsFilenameGetter = [](void *Data, int Idx, const char **OutText)
    {
        *OutText = ((ed_tilesets *)Data)[Idx].File;
        return true;
    };
    ImGui::Combo("Tileset", &CurrentTileset, TsFilenameGetter, State->Tilesets, State->TilesetsCount);
#endif
    r32 Scale = 2.0f;
    vec2 TileSize = vec2i(16, 16);
    vec2 ScaledTileSize = TileSize * Scale;
    vec2 ScaledAtlasSize = vec2i(256, 256) * Scale;

    ImGui::BeginChild("CanvasWindow##editor", vec2i(240, 400), true, ImGuiWindowFlags_HorizontalScrollbar);
    static ImDrawList *DrawList = ImGui::GetWindowDrawList();
    vec2 CanvasP = ImGui::GetCursorScreenPos();
    DrawList->AddRectFilled(CanvasP, CanvasP + ScaledAtlasSize, IM_COL32(255, 255, 255, 255));
    DrawList->AddImage(rndr::GetTextureID(State->AtlasTexture), CanvasP, CanvasP + ScaledAtlasSize, vec2i(0, 1), vec2i(1, 0));
    ImGui::InvisibleButton("canvas##editor", ScaledAtlasSize);
    static vec2 P;
    //static u32 SelectedTileX, SelectedTileY;
    if(ImGui::IsItemHovered() && ImGui::IsMouseClicked(0))
    {
        vec2 MouseP = ImGui::GetIO().MousePos;
        P = vec2(floor((MouseP.x - CanvasP.x) / ScaledTileSize.x), floor((MouseP.y - CanvasP.y) / ScaledTileSize.y));
    }
    for(r32 X = 0; X <= ScaledAtlasSize.x; X += ScaledTileSize.x)
    {
        DrawList->AddLine(CanvasP + vec2(X, 0), CanvasP + vec2(X, ScaledAtlasSize.y), 0x7D000000);
    }
    for(r32 Y = 0; Y <= ScaledAtlasSize.y; Y += ScaledTileSize.y)
    {
        DrawList->AddLine(CanvasP + vec2(0, Y), CanvasP + vec2(ScaledAtlasSize.x, Y), 0x7D000000);
    }
    ImGui::EndChild();
    
    if(State->EditorGrid[(u32)P.x][(u32)P.y] != nullptr)
    {
        ImGui::Text("Name=%s X = %d, Y = %d", State->EditorGrid[(u32)P.x][(u32)P.y]->Name, State->EditorGrid[(u32)P.x][(u32)P.y]->X, State->EditorGrid[(u32)P.x][(u32)P.y]->Y);
        State->SelectedTile = State->EditorGrid[(u32)P.x][(u32)P.y];
    }
    else
    {
        State->SelectedTile = nullptr;
    }
    /*
    for(int X = 0; X < 1024; ++X)
    {
        for(int Y = 0; Y < 1024; ++Y)
        {
            if(State->EditorGrid[X][Y] != nullptr)
            {
                //ImGui::Text("Tile X = %d, Y = %d", X, Y);
            }
        }
    }
    */
    ImGui::End();
#if 0
        r32 EditorTileScale = 3.0f;
        vec2 TileSize = vec2(8.0f) * EditorTileScale;
        vec2 TilesetSize = vec2(64, 96) * EditorTileScale;

        bool IsCanvasHovered;
        ImDrawList *DrawList = ImGui::GetWindowDrawList();
        vec2 CanvasPos = ImGui::GetCursorScreenPos();

        DrawList->AddImage(rndr::GetTextureID(GameState->EditorTilesetTexture), CanvasPos, CanvasPos + TilesetSize, ImVec2(0, 1), ImVec2(1, 0));
        ImGui::InvisibleButton("canvas", TilesetSize);
        IsCanvasHovered = ImGui::IsItemHovered();

        vec2 MousePos;
        vec2 HoveredTile;

        if(IsCanvasHovered)
        {
            if(ImGui::IsMouseClicked(0))
            {
                /*
                EdSelectedTileUV = vec4((HoverTile.x * TileSize.x)/TilesetSize.x,
                1.0f - (HoverTile.y * TileSize.y)/TilesetSize.y,
                ((HoverTile.x + 1) * TileSize.x)/TilesetSize.x,
                1.0f - ((HoverTile.y + 1) * TileSize.y)/TilesetSize.y);
                */
                MousePos = ImGui::GetIO().MousePos;;
                HoveredTile = vec2(floorf((MousePos.x - CanvasPos.x) / TileSize.x), floorf((MousePos.y - CanvasPos.y) / TileSize.y));
                SelTileUV = vec4((HoveredTile.x * TileSize.x)/TilesetSize.x,
                    1.0f - ((HoveredTile.y + 1) * TileSize.y)/TilesetSize.y,
                    ((HoveredTile.x + 1) * TileSize.x)/TilesetSize.x,
                    1.0f - (HoveredTile.y * TileSize.y)/TilesetSize.y);
            }
        }

        ImGui::TextUnformatted("Selected tile:\0");
        ImGui::SameLine();
        ImGui::Image(rndr::GetTextureID(GameState->EditorTilesetTexture), ImVec2(32, 32), ImVec2(SelTileUV.x, SelTileUV.w), ImVec2(SelTileUV.z, SelTileUV.y));

        for(r32 X = 0; X <= TilesetSize.x; X += TileSize.x)
        {
            DrawList->AddLine(CanvasPos + vec2(X, 0), CanvasPos + vec2(X, TilesetSize.y), 0x7D000000);
        }
        for(r32 Y = 0; Y <= TilesetSize.y; Y += TileSize.y)
        {
            DrawList->AddLine(CanvasPos + vec2(0, Y), CanvasPos + vec2(TilesetSize.x, Y), 0x7D000000);
        }

    }
    ImGui::End();

    rect Dest = Rect(10, 4, 1, 1);
    GameState->WorldVertices.clear();
    PushSprite(&GameState->WorldVertices, Dest, SelTileUV, vec4i(1, 1, 1, 1), 0.0f, vec2(0.0f), 1.0f);
    rndr::BufferData(GameState->WorldVertexBuffer, 0, (u32)sizeof(vert_P1C1UV1) * (u32)GameState->WorldVertices.size(), &GameState->WorldVertices.front());
#endif
}
