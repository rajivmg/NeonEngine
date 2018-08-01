#include "f_game.h"

#include "f_editor.h"
#include <dear-imgui/imgui.h>
#include <rapidxml/rapidxml.hpp>
using namespace rapidxml;

static game_state GameState = {};
static editor_state EditorState = {};

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
#if 0
    /* 
        Null       = 0
        Background = 1
        Wall       = 2
        Crate      = 3
        Player     = 4
    */ 
    static u32 Map[] =
    {
        2, 2, 2, 2, 2, 2, 2, 2, 2,
        2, 1, 1, 1, 1, 1, 1, 1, 2,
        2, 1, 1, 1, 1, 1, 1, 1, 2,
        2, 1, 1, 2, 2, 2, 1, 1, 2,
        2, 1, 3, 3, 2, 1, 1, 3, 2,
        2, 1, 1, 1, 1, 1, 1, 3, 2,
        2, 3, 1, 4, 1, 1, 1, 1, 2,
        2, 1, 1, 1, 1, 1, 1, 1, 2,
        2, 2, 2, 2, 2, 2, 2, 2, 2
    };

    // Background
    for(int X = 0; X < 9; ++X)
    {
        for(int Y = 0; Y < 9; ++Y)
        {
            GameState.Room.EntityMap[Y][X][0].Type = Entity_Background;
            GameState.Room.EntityMap[Y][X][0].P = vec3i(X, Y, 0);
        }
    }
    
    // Foreground
    for(int X = 0; X < 9; ++X)
    {
        for(int Y = 0; Y < 9; ++Y)
        {
            switch(Map[X + Y * 9])
            {
                case Entity_Wall:
                {
                    GameState.Room.EntityMap[Y][X][1].Type = Entity_Wall;
                    GameState.Room.EntityMap[Y][X][1].P = vec3i(X, Y, 1);
                } break;
                case Entity_Crate:
                {
                    GameState.Room.EntityMap[Y][X][1].Type = Entity_Crate;
                    GameState.Room.EntityMap[Y][X][1].P = vec3i(X, Y, 1);
                } break;
                case Entity_Player:
                {
                    GameState.Room.EntityMap[Y][X][1].Type = Entity_Player;
                    GameState.Room.EntityMap[Y][X][1].P = vec3i(X, Y, 1);
                    GameState.Room.Player = vec2i(X, Y);
                } break;
            }
        }
    }
#endif
}

//bool MoveEntity(room *Room, entity *Entity, vec2 Direction)
//{
//    entity *DestEntity = &GET_ENTITY(Room, Entity->X + (s32)Direction.x, Entity->Y + (s32)Direction.y, 1);
//    if(DestEntity->Type == GameObject_Null)
//    {
//        *DestEntity = *Entity;
//        Entity->Type = GameObject_Null;
//    }
//}

void GameUpdate(game_input *Input)
{
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
}

void GameSetup()
{
    // Set pointers and init renderer
    rndr::Init();

    // GameState
    GameState.MetersToPixels = Platform.WindowWidth / 16.0f; 
    GameState.PixelsToMeters = 1.0f / GameState.MetersToPixels;

    // Screen is 16x9 meters
    mat4 ViewMatrix = LookAt(vec3(0.0f), vec3(0.0f, 0.0f, -1.0f), vec3(0.0f, 1.0f, 0.0f));
    //mat4 ProjMatrix = Orthographic(0.0f, 16.0f, 9.0f, 0.0f, -10.0f, 0.0f);
    mat4 ProjMatrix = Orthographic(-5.0f, 5.0f, 5.0f, 0.0f, -10.0f, 1.0f);


    GameState.DbgFont = new font("fonts/dbg_font_26.fnt");

    bitmap IsoTileBitmap;
    //LoadBitmap(&IsoTileBitmap, "programmer_iso.tga");
    LoadBitmap(&IsoTileBitmap, "Ground_1.tga");


    GameState.IsoTileTexture = rndr::MakeTexture(&IsoTileBitmap, tex_param::TEX2D, tex_param::LINEAR, tex_param::CLAMP, true);
    
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

    LoadLevels();

    InitEditor(&EditorState);
    //vec4 Color = RGBAUnpack4x8(0x0C0C0CFF);
    int a = 0;
}

void GameUpdateAndRender(game_input *Input)
{
    //-----------------------------------------------------------------------------
    // Game Update
    //-----------------------------------------------------------------------------
    Editor(&EditorState);

    GameUpdate(Input);
    //-----------------------------------------------------------------------------
    // Game Render
    //-----------------------------------------------------------------------------
    rndr::Clear();

#if 0
    for(int X = 0; X < 9; ++X)
    {
        for(int Y = 0; Y < 9; ++Y)
        {
            for(int Z = 0; Z < 2; ++Z)
            {
                entity *Entity = &GameState.Room.EntityMap[Y][X][Z];
                switch(Entity->Type)
                {
                    case Entity_Background:
                    {
                        PushSprite(&GameState.GameVertices, Rect((r32)X, (r32)Y, 1.0f, 1.0f), vec4i(0,0,1,1), vec4i(1,1,1,1), 0.0f, vec2(0.0f), (r32)Z);
                    } break;
                    case Entity_Wall:
                    {
                        PushSprite(&GameState.GameVertices, Rect((r32)X, (r32)Y, 1.0f, 1.0f), vec4i(0,0,1,1), RGBA255To01(RGBAUnpack4x8(0x7354a8ff)), 0.0f, vec2(0.0f), (r32)Z);
                    } break;
                    case Entity_Crate:
                    {
                        PushSprite(&GameState.GameVertices, Rect((r32)X, (r32)Y, 1.0f, 1.0f), vec4i(0,0,1,1), RGBA255To01(RGBAUnpack4x8(0xa87154ff)), 0.0f, vec2(0.0f), (r32)Z);
                    } break;
                    case Entity_Player:
                    {
                        PushSprite(&GameState.GameVertices, Rect((r32)X, (r32)Y, 1.0f, 1.0f), vec4i(0,0,1,1), RGBA255To01(RGBAUnpack4x8(0xd6f441ff)), 0.0f, vec2(0.0f), (r32)Z);
                    } break;
                }
            }
        }
    }
#endif

    for(int X = 7; X >= 0; --X)
    {
        for(int Y = 7; Y >= 0; --Y)
        {
            game_object *Entity = &GameState.Level.Objects[Y][X][1];
            //r32 Sx = (X - Y) * 0.50f;
            //r32 Sy = (X + Y) * 0.25f;
            r32 Sx = (X - Y) * 1.0f;
            r32 Sy = (X + Y) * 0.25f;
            switch(Entity->Type)
            {
                case GameObject_Wall:
                {
                    PushSprite(&GameState.GameVertices, Rect((r32)Sx - 0.5f, (r32)Sy, 2.0f, 1.0f), vec4i(0,0,1,1), RGBA255To01(RGBAUnpack4x8(0x7354a8ff)), 0.0f, vec2(0.0f), (r32)1.0f);
                } break;
                case GameObject_Crate:
                {
                    PushSprite(&GameState.GameVertices, Rect((r32)Sx - 0.5f, (r32)Sy, 2.0f, 1.0f), vec4i(0,0,1,1), RGBA255To01(RGBAUnpack4x8(0xa87154ff)), 0.0f, vec2(0.0f), (r32)1.0f);
                } break;
                case GameObject_Player:
                {
                    PushSprite(&GameState.GameVertices, Rect((r32)Sx - 0.5f, (r32)Sy, 2.0f, 1.0f), vec4i(0,0,1,1), RGBA255To01(RGBAUnpack4x8(0xd6f441ff)), 0.0f, vec2(0.0f), (r32)1.0f);
                } break;
                default: //case GameObject_Floor:
                {
                    PushSprite(&GameState.GameVertices, Rect((r32)Sx - 0.5f, (r32)Sy, 2.0f, 1.0f), vec4i(0,0,1,1), vec4i(1, 1, 1, 1), 0.0f, vec2(0.0f), (r32)0.6f - Y/16.0f - X/16.0f);
                } break;
               /* default:
                { 
                    PushSprite(&GameState.GameVertices, Rect((r32)Sx - 0.5f, (r32)Sy, 1.0f, 1.0f), vec4i(0,0,1,1), vec4i(1,1,1,1), 0.0f, vec2(0.0f), (r32)1.0f);
                } break;*/
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
        WorldDrawCmd->Textures[0] = GameState.IsoTileTexture;

        GameState.GameVertices.clear();

        GameState.GameRender->Sort();
        GameState.GameRender->Submit();
        GameState.GameRender->Flush();
    }
#endif

    //PushText(&GameState.DbgTextVertices, Rect(600, 720, 1, 1), vec4i(1, 1, 0, 1), 1.0f, GameState.DbgFont, "A quick brown fox: %d\nA quick brown fox: %d\nJoe\nNEON", 1, 2);
    PushText(&GameState.DbgTextVertices, Rect(0, 720, 1, 1), vec4i(1, 1, 1, 1), 1.0f, GameState.DbgFont, "%0.2f ms/frame Framerate %ff/s", 1000.0 * Input->DeltaTime, 1 / Input->DeltaTime);
    PushText(&GameState.DbgTextVertices, Rect(400, 720, 1, 1), vec4i(1, 1, 1, 1), 1.0f, GameState.DbgFont, "Time %f", Input->Time);
   
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