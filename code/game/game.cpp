#include "game.h"

#include "editor.h"
#include <dear-imgui/imgui.h>
#include <rapidxml/rapidxml.hpp>
#include <rapidxml/rapidxml_print.hpp>
using namespace rapidxml;

static game_state GameState = {};
static editor_state EditorCtx = {};

#if 0
void LoadLevels()
{
    file_content LevelFile = Platform.ReadFile("levels.xml");
    ASSERT(LevelFile.NoError);

    // Null terminate xml file for parsing
    char *XmlFile = (char *)MALLOC(sizeof(char) * (LevelFile.Size + 1));
    memcpy(XmlFile, LevelFile.Content, LevelFile.Size + 1);
    XmlFile[LevelFile.Size] = '\0';

    Platform.FreeFileContent(&LevelFile);

    // Parse
    xml_document<> Doc;
    Doc.parse<0>(XmlFile);

    xml_node<> *LevelNode = Doc.first_node("level");
    xml_node<> *ObjectNode = LevelNode->first_node("object");
    xml_node<> *FirstYNode = ObjectNode->first_node("y");
    xml_node<> *YNode = FirstYNode;
    u32 Y = 7;
    while(YNode)
    {
        xml_attribute<> *YValueAttr = YNode->first_attribute("value");
        s32 V[9];
        sscanf(YValueAttr->value(), "%d, %d, %d, %d, %d, %d, %d, %d, %d", &V[0], &V[1], &V[2], &V[3], &V[4], &V[5], &V[6], &V[7], &V[8]);
        for(int X = 0; X < 8; ++X)
        {
            switch(V[X])
            {
                case GameObject_Null:
                {
                    GameState.Level.Objects[Y][X][1].Type = GameObject_Null;
                    GameState.Level.Objects[Y][X][1].X = X;
                    GameState.Level.Objects[Y][X][1].Y = Y;
                    GameState.Level.Objects[Y][X][1].P = vec2i(X, Y);
                } break;
                case GameObject_Wall:
                {
                    GameState.Level.Objects[Y][X][1].Type = GameObject_Wall;
                    GameState.Level.Objects[Y][X][1].X = X;
                    GameState.Level.Objects[Y][X][1].Y = Y;
                    GameState.Level.Objects[Y][X][1].P = vec2i(X, Y);
                } break;
                case GameObject_Crate:
                {
                    GameState.Level.Objects[Y][X][1].Type = GameObject_Crate;
                    GameState.Level.Objects[Y][X][1].X = X;
                    GameState.Level.Objects[Y][X][1].Y = Y;
                    GameState.Level.Objects[Y][X][1].P = vec2i(X, Y);
                } break;
                case GameObject_Player:
                {
                    GameState.Level.Objects[Y][X][1].Type = GameObject_Player;
                    GameState.Level.Objects[Y][X][1].X = X;
                    GameState.Level.Objects[Y][X][1].Y = Y;
                    GameState.Level.Objects[Y][X][1].P = vec2i(X, Y);
                    GameState.Level.Player = vec2i(X, Y);
                } break;
            }
        }
        --Y;
        YNode = YNode->next_sibling("y");
    }

    //// Background
    //for(int X = 0; X < 8; ++X)
    //{
    //    for(int Y = 0; Y < 8; ++Y)
    //    {
    //        GameState.Room.Objects[Y][X].Type = Entity_Background;
    //        GameState.Room.Objects[Y][X].P = vec2i(X, Y);
    //    }
    //}

    Doc.clear();
    SAFE_FREE(XmlFile);
}
#endif

void XMLWriteTest()
{
    xml_document<> Doc;
    xml_node<> *Decl = Doc.allocate_node(node_declaration);
    Decl->append_attribute(Doc.allocate_attribute("version", "1.0"));
    Doc.append_node(Decl);

    s32 LevelWidth = 22;
    char TempIntString[_MAX_ITOSTR_BASE10_COUNT];
    //char TempUintString[_MAX_ULTOSTR_BASE10_COUNT];
    xml_node<> *LevelNode = Doc.allocate_node(node_element, "level");
    LevelNode->append_attribute(Doc.allocate_attribute("name", "Prologue"));
    //LevelNode->append_attribute(Doc.allocate_attribute("width", "20"));
    _itoa(LevelWidth, TempIntString, 10);
    LevelNode->append_attribute(Doc.allocate_attribute("width", Doc.allocate_string(TempIntString)));
    Doc.append_node(LevelNode);

    xml_node<> *TileNode = Doc.allocate_node(node_element, "tile");
    TileNode->append_attribute(Doc.allocate_attribute("tid", "7"));
    LevelNode->append_node(TileNode);

    char Buffer[4096];
    char *EndOfPrint = rapidxml::print(Buffer, Doc, 0);
    *EndOfPrint = '\0';
    Doc.clear();
    Platform.WriteFile("TestXML.xml", (EndOfPrint - Buffer), Buffer);
}

void DrawText(rect Dest, vec4 Color, r32 Layer, font *Font, char const *Format, ...)
{
    u32 StartV, CountV;
    StartV = (u32)GameState.TextVertices.size();
    va_list ArgList;
    va_start(ArgList, Format);
    va_end(ArgList);

    PushText(&GameState.TextVertices, Dest, Color, Layer, Font, Format, ArgList);

    CountV = (u32)GameState.TextVertices.size() - StartV;

    cmd::draw *TextDrawCmd = GameState.TextRender->AddCommand<cmd::draw>(0);
    TextDrawCmd->VertexBuffer = GameState.TextVertexBuffer;
    TextDrawCmd->VertexFormat = vert_format::P1C1UV1;
    TextDrawCmd->StartVertex = StartV;
    TextDrawCmd->VertexCount = CountV;
    TextDrawCmd->Textures[0] = Font->Texture;
}

void GameSetup()
{
    rndr::Init(); // Important

    // GameState
    GameState.MetersToPixels = Platform.WindowWidth / 24.9f; 
    GameState.PixelsToMeters = 1.0f / GameState.MetersToPixels;

    // TODO: Setup a 4/3 viewport
    // 4/3 ratio
    GameState.GameProjMatrix = Orthographic(0.0f, 20.0f, 15.0f, 0.0f, -10.0f, 1.0f);
    GameState.GameViewMatrix = LookAt(vec3(0.0f), vec3i(0, 0, -1), vec3i(0, 1, 0));

    // Virtual screen resolution 1280x720
    GameState.ScreenProjMatrix = Orthographic(0.0f, 1280.0f, 720.0f, 0.0f, -10.0f, 1.0f);
    GameState.ScreenViewMatrix = LookAt(vec3(0.0f), vec3i(0, 0, -1), vec3i(0, 1, 0));

    GameState.SpriteShader = rndr::MakeShaderProgram("shaders/sprite_vs.glsl", "shaders/sprite_ps.glsl");
    GameState.TextShader = rndr::MakeShaderProgram("shaders/sprite_vs.glsl", "shaders/sprite_ps.glsl");
    GameState.DbgLineShader = rndr::MakeShaderProgram("shaders/debug_line_vs.glsl", "shaders/debug_line_ps.glsl");

    GameState.DbgTextVertexBuffer = rndr::MakeBuffer(resource_type::VERTEX_BUFFER, MEGABYTE(1), true);
    GameState.DbgTextRender = new render_cmd_list(MEGABYTE(1), GameState.TextShader, &GameState.ScreenViewMatrix, &GameState.ScreenProjMatrix);

    GameState.DbgLineVertexBuffer = rndr::MakeBuffer(resource_type::VERTEX_BUFFER, MEGABYTE(5), true);
    GameState.DbgLineRender = new render_cmd_list(MEGABYTE(1), GameState.DbgLineShader, &GameState.ScreenViewMatrix, &GameState.ScreenProjMatrix);

    GameState.SpriteVertexBuffer = rndr::MakeBuffer(resource_type::VERTEX_BUFFER, MEGABYTE(20), true);
    GameState.SpriteRender = new render_cmd_list(MEGABYTE(2), GameState.SpriteShader, &GameState.GameViewMatrix, &GameState.GameProjMatrix);

    GameState.TextVertexBuffer = rndr::MakeBuffer(resource_type::VERTEX_BUFFER, MEGABYTE(5), true);
    GameState.TextRender = new render_cmd_list(MEGABYTE(1), GameState.TextShader, &GameState.ScreenViewMatrix, &GameState.ScreenProjMatrix);
   
    //for(int X = 0; X < 20; ++X)
    //{
    //    for(int Y = 0; Y < 12; ++Y)
    //    {
    //        GameState.Level.Level[X][Y].ID = 65535;
    //    }
    //}

    //bitmap AtlasBitmap;
    //LoadBitmap(&AtlasBitmap, "Atlas.tga");
    //GameState.AtlasTexture = rndr::MakeTexture(&AtlasBitmap, tex_param::TEX2D, tex_param::NEAREST, tex_param::CLAMP, true);

    bitmap WhiteBitmap;
    LoadBitmap(&WhiteBitmap, "sprites/white_texture.tga");
    GameState.WhiteTexture = rndr::MakeTexture(&WhiteBitmap, tex_param::TEX2D, tex_param::NEAREST, tex_param::CLAMP, false);

    InitFont(&GameState.DbgFont,"fonts/inconsolata_26.fnt");
    InitFont(&GameState.MainFont, "fonts/zorque_42.fnt");

    InitEditor(&EditorCtx);
    GameState.GameMode = GameMode_Editor;
    XMLWriteTest();
}

void GameUpdateAndRender(game_input *Input)
{
    //-----------------------------------------------------------------------------
    // Game Update
    //-----------------------------------------------------------------------------

    //-----------------------------------------------------------------------------
    // Game Render
    //-----------------------------------------------------------------------------
    rndr::Clear();

    EditorUpdateAndRender(&EditorCtx, Input);

    PushText(&GameState.DbgTextVertices, Rect(0, 720, 1, 1), vec4i(1, 0, 1, 1), 1.0f, &GameState.DbgFont, "%0.2f ms/frame Framerate %ff/s", 1000.0 * Input->DeltaTime, 1 / Input->DeltaTime);
    //PushText(&GameState.DbgTextVertices, Rect(400, 720, 1, 1), vec4i(1, 1, 1, 1), 1.0f, GameState.DbgFont, "Time %f", Input->Time);
    //PushDbgLine(&GameState.DbgLineVertices, vec3(0, 0, 0), vec3(500, 500, 0), vec4i(1, 0, 0, 1));
    //DrawText(Rect(100, 500, 100, 100), vec4i(0, 0, 0, 1), 1.0f, &GameState.MainFont, "Dream of Cthulhu");
    
    // Game rendering commands
    if(!GameState.SpriteVertices.empty())
    {
        rndr::BufferData(GameState.SpriteVertexBuffer, 0, (u32)sizeof(vert_P1C1UV1) * (u32)GameState.SpriteVertices.size(), &GameState.SpriteVertices.front());

        cmd::draw *SpriteDrawCmd = GameState.SpriteRender->AddCommand<cmd::draw>(0);
        SpriteDrawCmd->VertexBuffer = GameState.SpriteVertexBuffer;
        SpriteDrawCmd->VertexFormat = vert_format::P1C1UV1;
        SpriteDrawCmd->StartVertex = 0;
        SpriteDrawCmd->VertexCount = (u32)GameState.SpriteVertices.size();
        SpriteDrawCmd->Textures[0] = GameState.AtlasTexture;

        GameState.SpriteVertices.clear();

        GameState.SpriteRender->Sort();
        GameState.SpriteRender->Submit();
        GameState.SpriteRender->Flush();
    }
    // Game text rendering commands
    if(!GameState.TextVertices.empty())
    {
        rndr::BufferData(GameState.TextVertexBuffer, 0, (u32)sizeof(vert_P1C1UV1) * (u32)GameState.TextVertices.size(), &GameState.TextVertices.front());
        GameState.TextVertices.clear();
        GameState.TextRender->Sort();
        GameState.TextRender->Submit();
        GameState.TextRender->Flush();
    }
    // Debug text rendering command
    if(!GameState.DbgTextVertices.empty())
    {
        rndr::BufferData(GameState.DbgTextVertexBuffer, 0, (u32)sizeof(vert_P1C1UV1) * (u32)GameState.DbgTextVertices.size(), &GameState.DbgTextVertices.front());

        cmd::draw *DebugTextCmd = GameState.DbgTextRender->AddCommand<cmd::draw>(0);
        DebugTextCmd->VertexBuffer = GameState.DbgTextVertexBuffer;
        DebugTextCmd->VertexFormat = vert_format::P1C1UV1;
        DebugTextCmd->StartVertex = 0;
        DebugTextCmd->VertexCount = (u32)GameState.DbgTextVertices.size();
        DebugTextCmd->Textures[0] = GameState.DbgFont.Texture;

        GameState.DbgTextVertices.clear();

        GameState.DbgTextRender->Sort();
        GameState.DbgTextRender->Submit();
        GameState.DbgTextRender->Flush();
    }
    // Debug line rendering command
    if(!GameState.DbgLineVertices.empty())
    {
        rndr::BufferData(GameState.DbgLineVertexBuffer, 0, (u32)sizeof(vert_P1C1) * (u32)GameState.DbgLineVertices.size(), &GameState.DbgLineVertices.front());

        cmd::draw_debug_lines *DrawDebugLinesCmd = GameState.DbgLineRender->AddCommand<cmd::draw_debug_lines>(0);
        DrawDebugLinesCmd->VertexBuffer = GameState.DbgLineVertexBuffer;
        DrawDebugLinesCmd->VertexFormat = vert_format::P1C1;
        DrawDebugLinesCmd->StartVertex = 0;
        DrawDebugLinesCmd->VertexCount = (u32)GameState.DbgLineVertices.size();

        GameState.DbgLineVertices.clear();

        GameState.DbgLineRender->Sort();
        GameState.DbgLineRender->Submit();
        GameState.DbgLineRender->Flush();
    }
}