#include "neon_editor.h"
#include <dear-imgui/imgui.h>
#include <rapidxml/rapidxml.hpp>
#include "neon_game.h"

static bool EditorInitialised = false;
static bool ShowTilesetCreator = false;

void ShowTilesetWindow()
{

}

static void InitEditor(editor_state *EditorState)
{
    EditorInitialised = true;
    EditorState->TsFilesCount = 2;
    EditorState->TsFileMax = 50;
    strncpy(EditorState->TsFiles[0], "sprites/cavesofgallet_tiles.tga", 64);
    strncpy(EditorState->TsFiles[1], "BBB", 64);
}

void EditorTick(game_state *GameState)
{
    editor_state *EditorState = &GameState->EditorState;
    //if(!EditorState->Initialised)
    //{
    //    InitEditor(EditorState);
    //}

    if(ShowTilesetCreator) { ShowTilesetWindow(); }

    static vec4 SelTileUV = vec4i(0, 0, 0, 0);
    ImGui::SetNextWindowSize(ImVec2(350, 560), ImGuiCond_FirstUseEver);
    if(!ImGui::Begin("Editor", 0, ImGuiWindowFlags_MenuBar))
    {
        ImGui::End();
        return;
    }
    {
        static int EditorMode = EDITOR_TILESET;

        if(ImGui::BeginMenuBar())
        {
            if(ImGui::BeginMenu("View"))
            {
                ImGui::MenuItem("Tileset", NULL, &ShowTilesetCreator);
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }
        //ImGui::Combo("Mode", &EditorMode, "Tileset Creation\0Edit\0");
        ImGui::Spacing();

        if(EditorMode == EDITOR_TILESET)
        {
            static int CurrentTileset = 0;
            auto TsFilenameGetter = [](void *Data, int Idx, const char **OutText)
            {
                *OutText = ((editor_state *)Data)->TsFiles[Idx];
                return true;
            };
            ImGui::Combo("Tilesets", &CurrentTileset, TsFilenameGetter, 0, EditorState->TsFilesCount);
            //ImGui::
        }

#if 1
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
#endif
    }
    ImGui::End();

    rect Dest = Rect(10, 4, 1, 1);
    GameState->WorldVertices.clear();
    PushSprite(&GameState->WorldVertices, Dest, SelTileUV, vec4i(1, 1, 1, 1), 0.0f, vec2(0.0f), 1.0f);
    rndr::BufferData(GameState->WorldVertexBuffer, 0, (u32)sizeof(vert_P1C1UV1) * (u32)GameState->WorldVertices.size(), &GameState->WorldVertices.front());
}
