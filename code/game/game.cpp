#include <dear-imgui/imgui.h>

// NOTE: Implementation of C2 collision detection library
#define CUTE_C2_IMPLEMENTATION
#include "cute_c2.h"

#include "editor.h"
#include "debugdraw.h"
#include "gamestate.h"
#include "map.h"

debug_draw *DebugDraw = nullptr; // NOTE: Global DebugDraw object pointer
game_state *GameState = nullptr;

static editor_state EditorCtx = {};

void DrawText(rect Dest, vec4 Color, r32 Layer, font *Font, char const *Format, ...)
{
    u32 StartV, CountV;
    StartV = (u32)GameState->TextVertices.size();

    va_list ArgList;
    va_start(ArgList, Format);
    PushTextV(&GameState->TextVertices, Dest, Color, Layer, Font, Format, ArgList);
    va_end(ArgList);

    CountV = (u32)GameState->TextVertices.size() - StartV;

    cmd::draw *TextDrawCmd = GameState->TextRender->AddCommand<cmd::draw>(0);
    TextDrawCmd->VertexBuffer = GameState->TextVertexBuffer;
    TextDrawCmd->VertexFormat = vert_format::P1C1UV1;
    TextDrawCmd->StartVertex = StartV;
    TextDrawCmd->VertexCount = CountV;
    TextDrawCmd->Textures[0] = Font->Texture;
}

static map_data Area1 = {};
static map Map = {};
void GameSetup()
{
    rndr::Init(); // NOTE: Initialize renderer

    GameState = new game_state(); // NOTE: Construct global GameState object
    DebugDraw = new debug_draw(); // NOTE: Construct global DebugDraw object

    GameState->StackAllocator.Init(MEGABYTE(128));
    GameState->EntityManager.Init();

    // TODO: Remove this
    GameState->MetersToPixels = Platform.WindowWidth / 32.0f; 
    GameState->PixelsToMeters = 1.0f / GameState->MetersToPixels;

    // NOTE: Game projection and view matrices
    //GameState->ProjMatrix = Orthographic(0.0f, 10.6666667f, 6.0f, 0.0f, -10.0f, 1.0f);
    GameState->ProjMatrix = Orthographic(0.0f, 32.0f, 18.0f, 0.0f, -10.0f, 1.0f);
    GameState->ViewMatrix = LookAt(vec3(0.0f), vec3i(0, 0, -1), vec3i(0, 1, 0));

    // NOTE: Virtual screen resolution 1280x720
    GameState->ScreenProjMatrix = Orthographic(0.0f, 1280.0f, 720.0f, 0.0f, -10.0f, 1.0f);
    GameState->ScreenViewMatrix = LookAt(vec3(0.0f), vec3i(0, 0, -1), vec3i(0, 1, 0));

    // NOTE: Make shaders
    GameState->SpriteShader = rndr::MakeShaderProgram("shaders/sprite_vs.glsl", "shaders/sprite_ps.glsl");
    GameState->TextShader = rndr::MakeShaderProgram("shaders/sprite_vs.glsl", "shaders/sprite_ps.glsl");

    // NOTE: Make Vertex buffers
    GameState->SpriteVertexBuffer = rndr::MakeBuffer(resource_type::VERTEX_BUFFER, MEGABYTE(20), true);
    GameState->SpriteRender = new render_cmd_list(MEGABYTE(2), GameState->SpriteShader, &GameState->ViewMatrix, &GameState->ProjMatrix);
    GameState->TextVertexBuffer = rndr::MakeBuffer(resource_type::VERTEX_BUFFER, MEGABYTE(5), true);
    GameState->TextRender = new render_cmd_list(MEGABYTE(1), GameState->TextShader, &GameState->ScreenViewMatrix, &GameState->ScreenProjMatrix);

    bitmap WhiteBitmap;
    LoadBitmap(&WhiteBitmap, "sprites/white_texture.tga");
    GameState->WhiteTexture = rndr::MakeTexture(&WhiteBitmap, tex_param::TEX2D, tex_param::NEAREST, tex_param::CLAMP, false);

    InitEditor(&EditorCtx);

    Area1.Init("map/Village.tmx");
    //Area1.PrepareStaticBuffers();

    Map.Init(&Area1);
    Area1.Shutdown();
    Map.PrepareStaticBuffer();
    int a = 0;
}

void GameUpdateAndRender(game_input *Input)
{
    GameState->EntityManager.Update(Input);
    GameState->EntityManager.Draw();
    
    //Area1.Render();
    Map.UpdateDynamicBuffer(Input);  // TODO: Move timer in GameState with pause functionality
    Map.Render();

    static vec3 CameraP = vec3(0, 0, 0);
    if(Input->Mouse.Left.EndedDown)
    {
        CameraP.x -= Input->Mouse.xrel * GameState->PixelsToMeters * 1.0f;
        CameraP.y -= Input->Mouse.yrel * GameState->PixelsToMeters * 1.0f;
    }

    GameState->ViewMatrix = LookAt(CameraP, CameraP + vec3i(0, 0, -1), vec3i(0, 1, 0));
    
    // NOTE: FPS Counter
    DebugDraw->Text(Rect(0, 720, 1, 1), RGBAUnpackTo01(0xa3f736ff), 10.0f, "%0.2f ms/frame Framerate %0.2f/s", 1000.0 * Input->DeltaTime, 1 / Input->DeltaTime);
    
    rndr::Clear(); // NOTE: Clear all framebuffers
 
    // NOTE: Sprite rendering commands
    //if(!GameState->SpriteVertices.empty())
    //{
    //    //GameState->SpriteVertices.clear();

        GameState->SpriteRender->Sort();
        GameState->SpriteRender->Submit();
        GameState->SpriteRender->Flush();
    //}
    
    // NOTE: Text rendering commands
    if(!GameState->TextVertices.empty())
    {
        rndr::BufferData(GameState->TextVertexBuffer, 0, (u32)sizeof(vert_P1C1UV1) * (u32)GameState->TextVertices.size(), &GameState->TextVertices.front());
        GameState->TextVertices.clear();
        GameState->TextRender->Sort();
        GameState->TextRender->Submit();
        GameState->TextRender->Flush();
    }

    // NOTE: Debug Draw commands
    DebugDraw->Submit();
}