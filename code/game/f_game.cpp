#include "f_game.h"

#include "f_editor.h"
#include <dear-imgui/imgui.h>
#include <rapidxml/rapidxml.hpp>
using namespace rapidxml;

static game_state GameState = {};
static editor_state EditorState = {};

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

void GameUpdate(game_input *Input)
{
#if 0
    game_controller_input *Controller = &Input->Controllers[0];
    level *Room = &GameState.Level;
    s32 PlayerX = (s32)Room->Player.x;
    s32 PlayerY = (s32)Room->Player.y;
    if(Controller->Left.EndedDown && Controller->Left.HalfTransitionCount == 1)
    {
        if(GET_ENTITY(Room, PlayerX-1, PlayerY, 1).Type == GameObject_Null)
        {
            GET_ENTITY(Room, PlayerX-1, PlayerY, 1) = GET_ENTITY(Room, PlayerX, PlayerY, 1);
            GET_ENTITY(Room, PlayerX, PlayerY, 1).Type = GameObject_Null;

            Room->Player += vec2i(-1, 0);
        }
    }
    if(Controller->Right.EndedDown && Controller->Right.HalfTransitionCount == 1)
    {
        if(GET_ENTITY(Room, PlayerX + 1, PlayerY, 1).Type == GameObject_Null)
        {
            GET_ENTITY(Room, PlayerX + 1, PlayerY, 1) = GET_ENTITY(Room, PlayerX, PlayerY, 1);
            GET_ENTITY(Room, PlayerX, PlayerY, 1).Type = GameObject_Null;

            Room->Player += vec2i(1, 0);
        }
    }
    if(Controller->Up.EndedDown && Controller->Up.HalfTransitionCount == 1)
    {
        if(GET_ENTITY(Room, PlayerX, PlayerY + 1, 1).Type == GameObject_Null)
        {
            GET_ENTITY(Room, PlayerX, PlayerY + 1, 1) = GET_ENTITY(Room, PlayerX, PlayerY, 1);
            GET_ENTITY(Room, PlayerX, PlayerY, 1).Type = GameObject_Null;

            Room->Player += vec2i(0, 1);
        }
    }
    if(Controller->Down.EndedDown && Controller->Down.HalfTransitionCount == 1)
    {
        if(GET_ENTITY(Room, PlayerX, PlayerY-1, 1).Type == GameObject_Null)
        {
            GET_ENTITY(Room, PlayerX, PlayerY - 1, 1) = GET_ENTITY(Room, PlayerX, PlayerY, 1);
            GET_ENTITY(Room, PlayerX, PlayerY, 1).Type = GameObject_Null;

            Room->Player += vec2i(0, -1);
        }
    }
#endif
}

void GameSetup()
{
    rndr::Init();

    // GameState
    GameState.MetersToPixels = Platform.WindowWidth / 24.9f; 
    GameState.PixelsToMeters = 1.0f / GameState.MetersToPixels;

    // Screen is 16x9 meters
    mat4 ViewMatrix = LookAt(vec3(0.0f), vec3(0.0f, 0.0f, -1.0f), vec3(0.0f, 1.0f, 0.0f));
    mat4 ProjMatrix = Orthographic(0.0f, 24.9f, 14.0f, 0.0f, -10.0f, 1.0f);
    //mat4 ProjMatrix = Orthographic(-5.0f, 5.0f, 5.0f, 0.0f, -10.0f, 1.0f);


    GameState.DbgFont = new font("fonts/dbg_font_26.fnt");

    bitmap AtlasBitmap;
    LoadBitmap(&AtlasBitmap, "AlphaAtlas.tga");
    GameState.AtlasTexture = rndr::MakeTexture(&AtlasBitmap, tex_param::TEX2D, tex_param::NEAREST, tex_param::CLAMP, true);
    
    bitmap WhiteBitmap;
    LoadBitmap(&WhiteBitmap, "sprites/white_texture.tga");
    GameState.WhiteTexture = rndr::MakeTexture(&WhiteBitmap, tex_param::TEX2D, tex_param::NEAREST, tex_param::CLAMP, false);

    GameState.SpriteShader = rndr::MakeShaderProgram("shaders/sprite_vs.glsl", "shaders/sprite_ps.glsl");

    GameState.TextShader = rndr::MakeShaderProgram("shaders/sprite_vs.glsl", "shaders/sprite_ps.glsl");
    //GameState.TextShader = rndr::MakeShaderProgram("shaders/text_vs.glsl", "shaders/text_ps.glsl");

    GameState.DbgTextVertexBuffer = rndr::MakeBuffer(resource_type::VERTEX_BUFFER, MEGABYTE(1), true);
    GameState.DbgTextRender = new render_cmd_list(MEGABYTE(1), GameState.TextShader);
    GameState.DbgTextRender->ViewMatrix = LookAt(vec3(0.0f), vec3i(0, 0, -1), vec3i(0, 1, 0));
    GameState.DbgTextRender->ProjMatrix = Orthographic(0.0f, (r32)Platform.WindowWidth, (r32)Platform.WindowHeight, 0.0f, -1.0f, 1.0f);

    GameState.DbgLineShader = rndr::MakeShaderProgram("shaders/debug_line_vs.glsl", "shaders/debug_line_ps.glsl");
    GameState.DbgLineVertexBuffer = rndr::MakeBuffer(resource_type::VERTEX_BUFFER, MEGABYTE(5), true);
    GameState.DbgLineRender = new render_cmd_list(MEGABYTE(1), GameState.DbgLineShader);
    GameState.DbgLineRender->ViewMatrix = ViewMatrix;
    GameState.DbgLineRender->ProjMatrix = ProjMatrix;

    GameState.GameRender = new render_cmd_list(MEGABYTE(2), GameState.SpriteShader);
    GameState.GameRender->ViewMatrix = ViewMatrix;
    GameState.GameRender->ProjMatrix = ProjMatrix;

    GameState.GameVertexBuffer = rndr::MakeBuffer(resource_type::VERTEX_BUFFER, MEGABYTE(20), true);

    //LoadLevels();
    for(int X = 0; X < 20; ++X)
    {
        for(int Y = 0; Y < 12; ++Y)
        {
            GameState.Level.Level[X][Y].ID = 65535;
        }
    }

    InitEditor(&EditorState);
    GameState.GameMode = GameMode_Editor;
    //vec4 Color = RGBAUnpack4x8(0x0C0C0CFF);
    int a = 0;
}

void GameUpdateAndRender(game_input *Input)
{
    //-----------------------------------------------------------------------------
    // Game Update
    //-----------------------------------------------------------------------------
    EditorUpdate(&EditorState);

    GameUpdate(Input);
    //-----------------------------------------------------------------------------
    // Game Render
    //-----------------------------------------------------------------------------
    rndr::Clear();

    // Draw tile grid
    for(int X = 0; X <= 20; ++X)
    {
        PushDbgLine(&GameState.DbgLineVertices, vec3i(X, 0, 0), vec3i(X, 12, 0), vec4i(1, 1, 0, 1));
    }
    for(int Y = 0; Y <= 12; ++Y)
    {
        PushDbgLine(&GameState.DbgLineVertices, vec3i(0, Y, 0), vec3i(20, Y, 0), vec4i(1, 1, 0, 1));
    }

    if(EditorState.SelectedTile != nullptr)
    {
        vec4 UV = vec4(EditorState.SelectedTile->X / 256.0f,
                      (256.0f - EditorState.SelectedTile->Y - 16) / 256.0f,
                      (EditorState.SelectedTile->X + 16) / 256.0f,
                      (256.0f - EditorState.SelectedTile->Y) / 256.0f);
        s32 X, Y;
        X = Clamp(0, (s32)floor(Input->Mouse.x * GameState.PixelsToMeters), 19);
        Y = Clamp(0, (s32)floor(Input->Mouse.y * GameState.PixelsToMeters), 11);
        
        if(Input->Mouse.Left.EndedDown && Input->Mouse.Left.HalfTransitionCount == 1)
        {
            PushText(&GameState.DbgTextVertices, Rect(100, 720, 1, 1), vec4i(1, 1, 1, 1), 1.0f, GameState.DbgFont, "Jj");
            GameState.Level.Level[X][Y].ID = (u16)EditorState.SelectedTile->ID;
            GameState.Level.Level[X][Y].UV = UV;
        }
        PushSprite(&GameState.GameVertices, Rect((r32)X, (r32)Y, 1.0f, 1.0f), UV, vec4(1.0f), 0.0f, vec2(0.0f), 1.0f);
    }

    for(int X = 0; X < 20; ++X)
    {
        for(int Y = 0; Y < 12; ++Y)
        {
            if(GameState.Level.Level[X][Y].ID != 65535)
            {
                PushSprite(&GameState.GameVertices, Rect((r32)X, (r32)Y, 1.0f, 1.0f), GameState.Level.Level[X][Y].UV, vec4(1.0f), 0.0f, vec2(0.0f), 1.0f);
            }
        }
    }

#if 1
    if(!GameState.GameVertices.empty())
    {
        rndr::BufferData(GameState.GameVertexBuffer, 0, (u32)sizeof(vert_P1C1UV1) * (u32)GameState.GameVertices.size(), &GameState.GameVertices.front());
        
        cmd::draw *WorldDrawCmd = GameState.GameRender->AddCommand<cmd::draw>(0);
        WorldDrawCmd->VertexBuffer = GameState.GameVertexBuffer;
        WorldDrawCmd->VertexFormat = vert_format::P1C1UV1;
        WorldDrawCmd->StartVertex = 0;
        WorldDrawCmd->VertexCount = (u32)GameState.GameVertices.size();
        WorldDrawCmd->Textures[0] = GameState.AtlasTexture;

        GameState.GameVertices.clear();

        GameState.GameRender->Sort();
        GameState.GameRender->Submit();
        GameState.GameRender->Flush();
    }
#endif

    //PushText(&GameState.DbgTextVertices, Rect(600, 720, 1, 1), vec4i(1, 1, 0, 1), 1.0f, GameState.DbgFont, "A quick brown fox: %d\nA quick brown fox: %d\nJoe\nNEON", 1, 2);
    PushText(&GameState.DbgTextVertices, Rect(900, 720, 1, 1), vec4i(1, 1, 1, 1), 1.0f, GameState.DbgFont, "%0.2f ms/frame Framerate %ff/s", 1000.0 * Input->DeltaTime, 1 / Input->DeltaTime);
    //PushText(&GameState.DbgTextVertices, Rect(400, 720, 1, 1), vec4i(1, 1, 1, 1), 1.0f, GameState.DbgFont, "Time %f", Input->Time);
   
    //PushDbgLine(&GameState.DbgLineVertices, vec3(0, 0, 0), vec3(500, 500, 0), vec4i(1, 0, 0, 1));

    if(!GameState.DbgTextVertices.empty())
    {
        rndr::BufferData(GameState.DbgTextVertexBuffer, 0, (u32)sizeof(vert_P1C1UV1) * (u32)GameState.DbgTextVertices.size(), &GameState.DbgTextVertices.front());

        cmd::draw *DebugTextCmd = GameState.DbgTextRender->AddCommand<cmd::draw>(0);
        DebugTextCmd->VertexBuffer = GameState.DbgTextVertexBuffer;
        DebugTextCmd->VertexFormat = vert_format::P1C1UV1;
        DebugTextCmd->StartVertex = 0;
        DebugTextCmd->VertexCount = (u32)GameState.DbgTextVertices.size();
        DebugTextCmd->Textures[0] = GameState.DbgFont->Texture;

        GameState.DbgTextVertices.clear();

        GameState.DbgTextRender->Sort();
        GameState.DbgTextRender->Submit();
        GameState.DbgTextRender->Flush();
    }

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