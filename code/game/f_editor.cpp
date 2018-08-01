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

    ImGui::Begin("Tileset Helper", &Show_TilesetHelperWindow, ImGuiWindowFlags_MenuBar);

    // Menu bar
    bool MenuFileOpen = false;
    if(ImGui::BeginMenuBar())
    {
        if(ImGui::BeginMenu("File"))
        {
            if(ImGui::MenuItem("Open", 0, false, !FileOpened)) { 
                MenuFileOpen = true; }

            ImGui::MenuItem("Close", 0, false, FileOpened);
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    if(MenuFileOpen) { ImGui::OpenPopup("Open File"); }

    // Menu bar End

    ImGui::Text("FILE: %s", Filename);
    if(!FileOpened)
    {
        ImGui::SameLine(ImGui::GetWindowWidth() - 60);
        if(ImGui::Button("OPEN"))
        {
            ImGui::OpenPopup("Open File");
        }
    }
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
    ImGui::Separator();

    // Tileset
    if(FileOpened)
    {
        r32 TileScale = 2.0f;
        vec2 TileSize = vec2(16.0f);
        vec2 TileScaledSize = TileSize * TileScale;
        vec2 TilesetSize = vec2i(TilesetBitmap.Width, TilesetBitmap.Height) * TileScale;

        static vec4 SelTileUV = vec4i(0, 0, 0, 0);
        bool IsCanvasHovered;
        ImDrawList *DrawList = ImGui::GetWindowDrawList();
        vec2 CanvasPos = ImGui::GetCursorScreenPos();

        DrawList->AddRectFilled(CanvasPos, CanvasPos + TilesetSize, IM_COL32(255, 255, 255, 255));
        DrawList->AddImage(rndr::GetTextureID(TilesetTexture), CanvasPos, CanvasPos + TilesetSize, ImVec2(0, 1), ImVec2(1, 0));
        ImGui::InvisibleButton("canvas", TilesetSize);
        IsCanvasHovered = ImGui::IsItemHovered();

        vec2 MousePos;
        vec2 HoveredTile;

        if(IsCanvasHovered)
        {
            if(ImGui::IsMouseClicked(0))
            {
                /*
                EdSelectedTileUV = vec4((HoverTile.x * TileScaledSize.x)/TilesetSize.x,
                1.0f - (HoverTile.y * TileScaledSize.y)/TilesetSize.y,
                ((HoverTile.x + 1) * TileScaledSize.x)/TilesetSize.x,
                1.0f - ((HoverTile.y + 1) * TileScaledSize.y)/TilesetSize.y);
                */
                MousePos = ImGui::GetIO().MousePos;;
                HoveredTile = vec2(floorf((MousePos.x - CanvasPos.x) / TileScaledSize.x), floorf((MousePos.y - CanvasPos.y) / TileScaledSize.y));
                SelTileUV = vec4((HoveredTile.x * TileScaledSize.x) / TilesetSize.x,
                    1.0f - ((HoveredTile.y + 1) * TileScaledSize.y) / TilesetSize.y,
                    ((HoveredTile.x + 1) * TileScaledSize.x) / TilesetSize.x,
                    1.0f - (HoveredTile.y * TileScaledSize.y) / TilesetSize.y);
            }
        }

        ImGui::TextUnformatted("Selected tile:\0");
        ImGui::SameLine();
        ImGui::Image(rndr::GetTextureID(TilesetTexture), ImVec2(32, 32), ImVec2(SelTileUV.x, SelTileUV.w), ImVec2(SelTileUV.z, SelTileUV.y));

        for(r32 X = 0; X <= TilesetSize.x; X += TileScaledSize.x)
        {
            DrawList->AddLine(CanvasPos + vec2(X, 0), CanvasPos + vec2(X, TilesetSize.y), 0x7D000000);
        }
        for(r32 Y = 0; Y <= TilesetSize.y; Y += TileScaledSize.y)
        {
            DrawList->AddLine(CanvasPos + vec2(0, Y), CanvasPos + vec2(TilesetSize.x, Y), 0x7D000000);
        }
    }
    // Tileset end

    ImGui::End();
}

void InitEditor(editor_state *State)
{
    file_content FileData = Platform.ReadFile("tilesets.xml");
    ASSERT(FileData.NoError);

    // Null terminate xml file for parsing
    State->TilesetsXmlFile = (char *)MALLOC(sizeof(char) * (FileData.Size + 1));
    memcpy(State->TilesetsXmlFile, FileData.Content, FileData.Size + 1);
    State->TilesetsXmlFile[FileData.Size] = '\0';

    Platform.FreeFileContent(&FileData);

    // Parse
    xml_document<> Doc;
    Doc.parse<0>(State->TilesetsXmlFile);

    xml_node<> *TilesetFirstNode = Doc.first_node("tileset");
    xml_node<> *TilesetNode = TilesetFirstNode;
    u32 TilesetIndex = 0;
    while(TilesetNode)
    {
        xml_attribute<> *TilesetIdAttr = TilesetNode->first_attribute("id");
        xml_attribute<> *TilesetFileAttr = TilesetNode->first_attribute("file");

        State->Tilesets[TilesetIndex].ID = strtoul(TilesetIdAttr->value(), nullptr, 10);
        strcpy(State->Tilesets[TilesetIndex].File, TilesetFileAttr->value());

        ++TilesetIndex;
        TilesetNode = TilesetNode->next_sibling("tileset");
    }
    State->TilesetsCount = TilesetIndex;
}

void Editor(editor_state *State)
{
    if(Show_TilesetHelperWindow) TilesetHelperWindow(State);

    static vec4 SelTileUV = vec4i(0, 0, 0, 0);
    ImGui::SetNextWindowSize(ImVec2(350, 560), ImGuiCond_FirstUseEver);
    if(!ImGui::Begin("Editor", 0, ImGuiWindowFlags_MenuBar))
    {
        ImGui::End();
        return;
    }
    {
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

        static int CurrentTileset = 0;
        auto TsFilenameGetter = [](void *Data, int Idx, const char **OutText)
        {
            *OutText = ((ed_tilesets *)Data)[Idx].File;
            return true;
        };
        ImGui::Combo("Tileset", &CurrentTileset, TsFilenameGetter, State->Tilesets, State->TilesetsCount);
    }
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
