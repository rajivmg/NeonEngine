#include "editor.h"

#include <core/neon_bitmap.h>
#include <dear-imgui/imgui.h>

#include <rapidxml/rapidxml.hpp>
#include <rapidxml/rapidxml_print.hpp>
using namespace rapidxml;

#include "game.h"

static bool Show_TilesetHelperWindow = false;

static void SaveLevel(editor_state *EditorState)
{
    xml_document<> Doc;
    xml_node<> *Decl = Doc.allocate_node(node_declaration);
    Decl->append_attribute(Doc.allocate_attribute("version", "1.0"));
    Doc.append_node(Decl);

    char TempIntString[_MAX_ITOSTR_BASE10_COUNT];
    char TempUintString[_MAX_ULTOSTR_BASE10_COUNT];

    xml_node<> *LevelNode = Doc.allocate_node(node_element, "level");
    LevelNode->append_attribute(Doc.allocate_attribute("name", EditorState->EditLevel.Name));
    _itoa(EditorState->EditLevel.Width, TempIntString, 10);
    LevelNode->append_attribute(Doc.allocate_attribute("width", Doc.allocate_string(TempIntString)));
    _itoa(EditorState->EditLevel.Height, TempIntString, 10);
    LevelNode->append_attribute(Doc.allocate_attribute("height", Doc.allocate_string(TempIntString)));
    Doc.append_node(LevelNode);

    for(int Y = 0; Y < EditorState->EditLevel.Height; ++Y)
    {
        for(int X = 0; X < EditorState->EditLevel.Width; ++X)
        {
            xml_node<> *TileNode = Doc.allocate_node(node_element, "tile");
            _ultoa(EditorState->EditLevel.Tiles[X][Y], TempUintString, 10);
            TileNode->append_attribute(Doc.allocate_attribute("tid", Doc.allocate_string(TempUintString)));
            LevelNode->append_node(TileNode);
        }
    }

    char *Buffer = (char *)MALLOC(MEGABYTE(1));
    char *EndOfPrint = rapidxml::print(Buffer, Doc, 0);
    *EndOfPrint = '\0';
    Doc.clear();
    Platform.WriteFile(EditorState->EditLevel.Filename, (EndOfPrint - Buffer), Buffer);
    SAFE_FREE(Buffer);
}

static void LoadLevel(const char *Filename, editor_state *EditorState)
{
    file_content File = Platform.ReadFile(Filename);
    ASSERT(LevelFile.NoError);

    // Null terminate xml file for parsing
    char *XmlFile = (char *)MALLOC(sizeof(char) * (File.Size + 1));
    memcpy(XmlFile, File.Content, File.Size + 1);
    XmlFile[File.Size] = '\0';

    Platform.FreeFileContent(&File);

    xml_document<> Doc;
    Doc.parse<0>(XmlFile);

    xml_node<> *LevelNode = Doc.first_node("level");
    ASSERT(LevelNode);
    xml_attribute<> *WidthAttr = LevelNode->first_attribute("width");
    xml_attribute<> *HeightAttr = LevelNode->first_attribute("height");
    EditorState->EditLevel.Width = strtol(WidthAttr->value(), nullptr, 10);
    EditorState->EditLevel.Height = strtol(HeightAttr->value(), nullptr, 10);
    
    xml_node<> *TileFirstNode = LevelNode->first_node("tile");
    xml_node<> *TileNode = TileFirstNode;

    for(int Y = 0; Y < EditorState->EditLevel.Height; ++Y)
    {
        for(int X = 0; X < EditorState->EditLevel.Width; ++X)
        {
            xml_attribute<> *TIDAttr = TileNode->first_attribute("tid");
            EditorState->EditLevel.Tiles[X][Y] = (u16)strtoul(TIDAttr->value(), nullptr, 10);
            TileNode = TileNode->next_sibling("tile");
        }
    }

    Doc.clear();
    SAFE_FREE(XmlFile);
}

static void TilesetHelperWindow(editor_state *EditorState)
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

    // Tileset
    if(FileOpened)
    {
        ImGui::Columns(2, "TilesetHelperWindowColumns", true);
        ImGui::BeginChild("CanvasWindow", vec2i(400, 400), true, ImGuiWindowFlags_HorizontalScrollbar);

        static r32 TileScale = 2.0f;
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
        //static char TileName[64] = "dummy";
        static s32  TileID = 0;
        ImGui::InputInt("Tile ID", &TileID);
        //ImGui::InputText("Tile Name", TileName, 64);
        sprintf(TileText, "<tile id=\"%d\" x=\"%d\" y=\"%d\" w=\"%d\" h=\"%d\"/>", TileID, (s32)SelectedTileP.x, (s32)SelectedTileP.y, (s32)TileSize.x, (s32)TileSize.y);
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

void InitEditor(editor_state *EditorState)
{
    // Initialise editor state and buffers
    EditorState->MetersToPixels = Platform.WindowWidth / 24.8889f; 
    EditorState->PixelsToMeters = 1.0f / EditorState->MetersToPixels;
    EditorState->CameraP = vec3(0.0f);

    EditorState->ViewMatrix = LookAt(EditorState->CameraP, EditorState->CameraP + vec3i(0, 0, -1), vec3i(0, 1, 0));
    EditorState->ProjMatrix = Orthographic(0.0f, 24.8889f, 14.0f, 0.0f, -10.0f, 1.0f);

    EditorState->DbgLineShader = rndr::MakeShaderProgram("shaders/debug_line_vs.glsl", "shaders/debug_line_ps.glsl");
    EditorState->DbgLineVertexBuffer = rndr::MakeBuffer(resource_type::VERTEX_BUFFER, MEGABYTE(5), true);
    EditorState->DbgLineRender = new render_cmd_list(MEGABYTE(1), EditorState->DbgLineShader, &EditorState->ViewMatrix, &EditorState->ProjMatrix);
    
    EditorState->SpriteShader = rndr::MakeShaderProgram("shaders/sprite_vs.glsl", "shaders/sprite_ps.glsl");
    EditorState->SpriteVertexBuffer = rndr::MakeBuffer(resource_type::VERTEX_BUFFER, MEGABYTE(16), true);
    EditorState->SpriteRender = new render_cmd_list(MEGABYTE(5), EditorState->SpriteShader, &EditorState->ViewMatrix, &EditorState->ProjMatrix);

    //EditorState->LevelWidth = 24; EditorState->LevelHeight = 14;
    //EditorState->EditLevel = (editor_game_tile *)MALLOC(sizeof(editor_game_tile) * EditorState->LevelWidth * EditorState->LevelHeight);

    //for(int X = 0; X < EditorState->LevelWidth; ++X)
    //{
    //    for(int Y = 0; Y < EditorState->LevelHeight; ++Y)
    //    {
    //        editor_game_tile *Tile = (EditorState->EditLevel + X + (Y * EditorState->LevelWidth));
    //        Tile->ID = U16_MAX;
    //        Tile->Collide = false;
    //    }
    //}

    memset(&EditorState->EditLevel, 0, sizeof(editor_level));
    EditorState->EditLevel.Width = 20; EditorState->EditLevel.Height= 15;
    strcpy(EditorState->EditLevel.Name, "Prologue");
    strcpy(EditorState->EditLevel.Filename, "Prologue.xml");
    EditorState->BrushTile = nullptr;
    EditorState->EditMode = EditMode_Brush;

    // Load tileset file
    file_content FileData = Platform.ReadFile("Data.xml");
    ASSERT(FileData.NoError);

    // Null terminate xml file for parsing
    EditorState->TilesetXmlFile = (char *)MALLOC(sizeof(char) * (FileData.Size + 1));
    memcpy(EditorState->TilesetXmlFile, FileData.Content, FileData.Size + 1);
    EditorState->TilesetXmlFile[FileData.Size] = '\0';

    Platform.FreeFileContent(&FileData);

    // Parse
    xml_document<> Doc;
    Doc.parse<0>(EditorState->TilesetXmlFile);

    xml_node<> *TilesetNode = Doc.first_node("tiledata");
    xml_attribute<> *TilesetBitmapAttr = TilesetNode->first_attribute("bitmap");
    xml_attribute<> *TilesetCountAttr = TilesetNode->first_attribute("count");
    
    bitmap AtlasBitmap;
    LoadBitmap(&AtlasBitmap, TilesetBitmapAttr->value());
    EditorState->SRGBAtlasTexture = rndr::MakeTexture(&AtlasBitmap, tex_param::TEX2D, tex_param::NEAREST, tex_param::CLAMP, false);
    EditorState->AtlasTexture = rndr::MakeTexture(&AtlasBitmap, tex_param::TEX2D, tex_param::NEAREST, tex_param::CLAMP, true);
    EditorState->AtlasWidth = (r32)AtlasBitmap.Width;
    EditorState->AtlasHeight = (r32)AtlasBitmap.Height;
    FreeBitmap(&AtlasBitmap);

    s32 TileCount = strtol(TilesetCountAttr->value(), nullptr, 10);
    EditorState->EditorTiles = (editor_tile *)MALLOC(sizeof(editor_tile) * TileCount);
    Platform.Log("Tile Count: %d", TileCount);
    xml_node<> *TileNode = TilesetNode->first_node("tile");
    s32 TileCounter = 0;
    while(TileNode)
    {
        xml_attribute<> *TileIdAttr = TileNode->first_attribute("id");
        xml_attribute<> *TileXAttr = TileNode->first_attribute("x");
        xml_attribute<> *TileYAttr = TileNode->first_attribute("y");
        xml_attribute<> *TileWAttr = TileNode->first_attribute("w");
        xml_attribute<> *TileHAttr = TileNode->first_attribute("h");

        EditorState->EditorTiles[TileCounter].ID = strtoul(TileIdAttr->value(), nullptr, 10);
        EditorState->EditorTiles[TileCounter].X = strtoul(TileXAttr->value(), nullptr, 10);
        EditorState->EditorTiles[TileCounter].Y = strtoul(TileYAttr->value(), nullptr, 10);
        EditorState->EditorTiles[TileCounter].W = strtoul(TileWAttr->value(), nullptr, 10);
        EditorState->EditorTiles[TileCounter].H = strtoul(TileHAttr->value(), nullptr, 10);

        u32 X = EditorState->EditorTiles[TileCounter].X / 16;
        u32 Y = EditorState->EditorTiles[TileCounter].Y / 16;
        EditorState->EditorGrid[X][Y] = &EditorState->EditorTiles[TileCounter];

        ++TileCounter;
        TileNode = TileNode->next_sibling("tile");
    }

    //ASSERT(TileCount == TileCounter);
}

void EditorUpdateAndRender(editor_state *EditorState, game_input *GameInput)
{
    // Begin ImGui
    if(Show_TilesetHelperWindow) { TilesetHelperWindow(EditorState); }

    ImGui::SetNextWindowSize(ImVec2(305, 720), ImGuiCond_Always);
    ImGui::SetNextWindowPos(ImVec2((r32)Platform.WindowWidth - 305, 0), ImGuiCond_Always);
    if(!ImGui::Begin("Editor", 0, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse))
    {
        ImGui::End();
        return;
    }

    bool MenuNewLevel = false;
    bool MenuOpenLevel = false;
    bool MenuSaveLevel = false;
    bool MenuCloseLevel = false;

    static bool LevelOpened = false;
    // Editor Menu Bar
    if(ImGui::BeginMenuBar())
    {
        if(ImGui::BeginMenu("File"))
        {
            if(ImGui::MenuItem("New Level", 0, false, true)) { MenuNewLevel = true; }
            if(ImGui::MenuItem("Open Level", 0, false, !LevelOpened)) { MenuOpenLevel = true; }
            if(ImGui::MenuItem("Save Level", 0, false, LevelOpened)) { MenuSaveLevel = true; }
            if(ImGui::MenuItem("Close Level", 0, false, LevelOpened)) { MenuCloseLevel = true; }
            ImGui::EndMenu();
        }
        if(ImGui::BeginMenu("View"))
        {
            ImGui::MenuItem("Tileset Helper", 0, &Show_TilesetHelperWindow);
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    if(MenuNewLevel) { ImGui::OpenPopup("New Level"); }
    if(MenuOpenLevel) { ImGui::OpenPopup("Open Level"); }
    if(MenuSaveLevel) { ImGui::OpenPopup("Save Level"); }
    if(MenuCloseLevel) { ImGui::OpenPopup("Close Level"); }

    static char LevelFilename[256];
    static file_content LevelFileContent;
    static char *ErrMsg = nullptr;

    if(ImGui::BeginPopupModal("Open Level", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::InputText("File", LevelFilename, 128);
        if(ErrMsg)
        {
            ImGui::Text("%s", ErrMsg);
        }
        if(ImGui::Button("OPEN"))
        {
            LevelFileContent = Platform.ReadFile(LevelFilename);
            if(!LevelFileContent.NoError)
            {
                ErrMsg = "Error reading file.";
            }
            else
            {
                LevelOpened = true;
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

    if(ImGui::BeginPopupModal("Close Level", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Are you sure?");
        if(ImGui::Button("YES"))
        {
            ASSERT(LevelOpened);

            LevelFilename[0] = '\0';
            LevelOpened = false;
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if(ImGui::Button("NO"))
        {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

    if(MenuSaveLevel)
    {
        ASSERT(LevelOpened);

    }

    ImGui::Spacing();
    ImGui::Text("LEVEL: %s", LevelFilename);

    // Mouse hover check
    if(ImGui::IsWindowHovered())
    {
        EditorState->WindowHovered = true;
    }
    else
    {
        EditorState->WindowHovered = false;
    }

    r32 Scale = 2.0f;
    vec2 TileSize = vec2i(16, 16);
    vec2 ScaledTileSize = TileSize * Scale;
    vec2 ScaledAtlasSize = vec2(EditorState->AtlasWidth, EditorState->AtlasHeight) * Scale;

    ImGui::BeginChild("CanvasWindow##editor", vec2i(288, 288), true, ImGuiWindowFlags_HorizontalScrollbar); // 288, 416
    static ImDrawList *DrawList = ImGui::GetWindowDrawList();
    vec2 CanvasP = ImGui::GetCursorScreenPos();
    DrawList->AddRectFilled(CanvasP, CanvasP + ScaledAtlasSize, IM_COL32(240, 240, 240, 255));
    DrawList->AddImage(rndr::GetTextureID(EditorState->SRGBAtlasTexture), CanvasP, CanvasP + ScaledAtlasSize, vec2i(0, 1), vec2i(1, 0));
    ImGui::InvisibleButton("canvas##editor", ScaledAtlasSize);

    static s32 STileX = 0, STileY = 0;
    if(ImGui::IsItemHovered() && ImGui::IsMouseClicked(0))
    {
        vec2 MouseP = ImGui::GetIO().MousePos;
        STileX = (s32)floor((MouseP.x - CanvasP.x) / ScaledTileSize.x);
        STileY = (s32)floor((MouseP.y - CanvasP.y) / ScaledTileSize.y);
    }
#if 1
    // Draw grid
    for(r32 X = 0; X <= ScaledAtlasSize.x; X += ScaledTileSize.x)
    {
        DrawList->AddLine(CanvasP + vec2(X, 0), CanvasP + vec2(X, ScaledAtlasSize.y), 0x7D777777);
    }
    for(r32 Y = 0; Y <= ScaledAtlasSize.y; Y += ScaledTileSize.y)
    {
        DrawList->AddLine(CanvasP + vec2(0, Y), CanvasP + vec2(ScaledAtlasSize.x, Y), 0x7D777777);
    }
#endif
    // TODO: Fix bug, when window is hovered tile brush still paints sometimes
    // Mouse hover check
    if(!EditorState->WindowHovered && ImGui::IsWindowHovered())
    {
        EditorState->WindowHovered = true;
    }

    if(EditorState->EditorGrid[STileX][STileY] != nullptr)
    {
        editor_tile *Tile = EditorState->EditorGrid[STileX][STileY];
        EditorState->BrushTile = EditorState->EditorGrid[STileX][STileY];
        //ImGui::Text("%s %d,%d", Tile->Name, Tile->X, Tile->Y);

        DrawList->AddLine(CanvasP + vec2i(STileX, STileY) * ScaledTileSize, CanvasP + vec2i(STileX + 1, STileY) * ScaledTileSize, IM_COL32(0, 255, 00, 255));
        DrawList->AddLine(CanvasP + vec2i(STileX + 1, STileY) * ScaledTileSize, CanvasP + vec2i(STileX + 1, STileY + 1) * ScaledTileSize, IM_COL32(0, 255, 00, 255));
        DrawList->AddLine(CanvasP + vec2i(STileX + 1, STileY + 1) * ScaledTileSize, CanvasP + vec2i(STileX, STileY + 1) * ScaledTileSize, IM_COL32(0, 255, 00, 255));
        DrawList->AddLine(CanvasP + vec2i(STileX, STileY + 1) * ScaledTileSize, CanvasP + vec2i(STileX, STileY) * ScaledTileSize, IM_COL32(0, 255, 00, 255));
    }
    else
    {
        EditorState->BrushTile = nullptr;
        DrawList->AddLine(CanvasP + vec2i(STileX, STileY) * ScaledTileSize, CanvasP + vec2i(STileX + 1, STileY) * ScaledTileSize, IM_COL32(255, 0, 00, 255));
        DrawList->AddLine(CanvasP + vec2i(STileX + 1, STileY) * ScaledTileSize, CanvasP + vec2i(STileX + 1, STileY + 1) * ScaledTileSize, IM_COL32(255, 0, 00, 255));
        DrawList->AddLine(CanvasP + vec2i(STileX + 1, STileY + 1) * ScaledTileSize, CanvasP + vec2i(STileX, STileY + 1) * ScaledTileSize, IM_COL32(255, 0, 00, 255));
        DrawList->AddLine(CanvasP + vec2i(STileX, STileY + 1) * ScaledTileSize, CanvasP + vec2i(STileX, STileY) * ScaledTileSize, IM_COL32(255, 0, 00, 255));
    }

    ImGui::EndChild();

    ImGui::Spacing();
    ImGui::Text("Edit Mode");
    ImGui::RadioButton("Brush", (int *)&EditorState->EditMode, (int)EditMode_Brush); ImGui::SameLine();
    ImGui::RadioButton("Erase", (int *)&EditorState->EditMode, (int)EditMode_Erase);

    if(ImGui::Button("Save Level"))
    {
        SaveLevel(EditorState);
    }

    if(ImGui::Button("Load Level"))
    {
        LoadLevel("Prologue.xml", EditorState);
    }

    ImGui::End(); // End ImGui


    // In-Game editor 
    if(!EditorState->WindowHovered && GameInput->Mouse.Right.EndedDown)
    {
        EditorState->CameraP.x -= GameInput->Mouse.xrel * EditorState->PixelsToMeters * 1.0f;
        EditorState->CameraP.y -= GameInput->Mouse.yrel * EditorState->PixelsToMeters * 1.0f;
    }

    EditorState->ViewMatrix = LookAt(EditorState->CameraP, EditorState->CameraP + vec3i(0, 0, -1), vec3i(0, 1, 0));

    // Brush
    if(!EditorState->WindowHovered && EditorState->EditMode == EditMode_Brush && EditorState->BrushTile != nullptr)
    {
        vec4 UV = vec4(EditorState->BrushTile->X / EditorState->AtlasWidth,
            (EditorState->AtlasHeight - EditorState->BrushTile->Y - TileSize.y) / EditorState->AtlasHeight,
            (EditorState->BrushTile->X + TileSize.x) / EditorState->AtlasWidth,
            (EditorState->AtlasHeight - EditorState->BrushTile->Y) / EditorState->AtlasHeight);
        s32 X, Y;
        X = Clamp(0, (s32)floor(EditorState->CameraP.x + GameInput->Mouse.x * EditorState->PixelsToMeters), EditorState->EditLevel.Width - 1);
        Y = Clamp(0, (s32)floor(EditorState->CameraP.y + GameInput->Mouse.y * EditorState->PixelsToMeters), EditorState->EditLevel.Height - 1);

        PushSprite(&EditorState->SpriteVertices, Rect((r32)X, (r32)Y, 1.0f, 1.0f), UV, vec4(1.0f), 0.0f, vec2(0.0f), 1.0f);

        if(GameInput->Mouse.Left.EndedDown)
        {
            if(EditorState->EditLevel.Tiles[X][Y] != EditorState->BrushTile->ID)
            {
                EditorState->EditLevel.Tiles[X][Y] = EditorState->BrushTile->ID;
            }
        }
    }

    // Erase
    if(!EditorState->WindowHovered && EditorState->EditMode == EditMode_Erase)
    {
        s32 X, Y;
        X = Clamp(0, (s32)floor(EditorState->CameraP.x + GameInput->Mouse.x * EditorState->PixelsToMeters), EditorState->EditLevel.Width - 1);
        Y = Clamp(0, (s32)floor(EditorState->CameraP.y + GameInput->Mouse.y * EditorState->PixelsToMeters), EditorState->EditLevel.Height - 1);

        if(GameInput->Mouse.Left.EndedDown)
        {
            if(EditorState->EditLevel.Tiles[X][Y] != 0)
            {
                EditorState->EditLevel.Tiles[X][Y] = 0;
            }
        }
    }

    // Draw tile grid
    for(int X = 0; X <= EditorState->EditLevel.Width; ++X)
    {
        PushDbgLine(&EditorState->DbgLineVertices, vec3((r32)X, 0, 0), vec3((r32)X, (r32)EditorState->EditLevel.Height, 0), RGBAUnpackTo01(0x777777ff));
    }
    for(int Y = 0; Y <= EditorState->EditLevel.Height; ++Y)
    {
        PushDbgLine(&EditorState->DbgLineVertices, vec3(0, (r32)Y, 0), vec3((r32)EditorState->EditLevel.Width, (r32)Y, 0), RGBAUnpackTo01(0x777777ff));
    }

    // Draw edit-level
    for(int Y = 0; Y < EditorState->EditLevel.Height; ++Y)
    {
        for(int X = 0; X < EditorState->EditLevel.Width; ++X)
        {
            u16 TID = EditorState->EditLevel.Tiles[X][Y];
            if(TID != 0)
            {
                editor_tile *Tile = &EditorState->EditorTiles[TID - 1];
                vec4 UV = vec4(Tile->X / EditorState->AtlasWidth, (EditorState->AtlasHeight - Tile->Y - TileSize.y) / EditorState->AtlasHeight,
                    (Tile->X + TileSize.x) / EditorState->AtlasWidth, (EditorState->AtlasHeight - Tile->Y) / EditorState->AtlasHeight);
                PushSprite(&EditorState->SpriteVertices, Rect((r32)X, (r32)Y, 1.0f, 1.0f), UV, vec4(1.0f), 0.0f, vec2(0.0f), 1.0f);
            }
        }
    }

    // Dispatch buffers for rendering 
    if(!EditorState->SpriteVertices.empty())
    {
        rndr::BufferData(EditorState->SpriteVertexBuffer, 0, (u32)sizeof(vert_P1C1UV1) * (u32)EditorState->SpriteVertices.size(), &EditorState->SpriteVertices.front());

        cmd::draw *WorldDrawCmd = EditorState->SpriteRender->AddCommand<cmd::draw>(0);
        WorldDrawCmd->VertexBuffer = EditorState->SpriteVertexBuffer;
        WorldDrawCmd->VertexFormat = vert_format::P1C1UV1;
        WorldDrawCmd->StartVertex = 0;
        WorldDrawCmd->VertexCount = (u32)EditorState->SpriteVertices.size();
        WorldDrawCmd->Textures[0] = EditorState->AtlasTexture;

        EditorState->SpriteVertices.clear();

        EditorState->SpriteRender->Sort();
        EditorState->SpriteRender->Submit();
        EditorState->SpriteRender->Flush();
    }
    if(!EditorState->DbgLineVertices.empty())
    {
        rndr::BufferData(EditorState->DbgLineVertexBuffer, 0, (u32)sizeof(vert_P1C1) * (u32)EditorState->DbgLineVertices.size(), &EditorState->DbgLineVertices.front());

        cmd::draw_debug_lines *DrawDebugLinesCmd = EditorState->DbgLineRender->AddCommand<cmd::draw_debug_lines>(0);
        DrawDebugLinesCmd->VertexBuffer = EditorState->DbgLineVertexBuffer;
        DrawDebugLinesCmd->VertexFormat = vert_format::P1C1;
        DrawDebugLinesCmd->StartVertex = 0;
        DrawDebugLinesCmd->VertexCount = (u32)EditorState->DbgLineVertices.size();

        EditorState->DbgLineVertices.clear();

        EditorState->DbgLineRender->Sort();
        EditorState->DbgLineRender->Submit();
        EditorState->DbgLineRender->Flush();
    }
}