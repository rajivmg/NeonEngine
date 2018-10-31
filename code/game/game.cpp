#include <dear-imgui/imgui.h>

// NOTE: Implementation of C2 collision detection library
#define CUTE_C2_IMPLEMENTATION
#include "cute_c2.h"

#include "editor.h"
#include "debugdraw.h"
#include "gamestate.h"

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

void GameSetup()
{
    rndr::Init(); // NOTE: Initialize renderer

    GameState = new game_state(); // NOTE: Construct global GameState object
    DebugDraw = new debug_draw(); // NOTE: Construct global DebugDraw object

    GameState->StackAllocator.Init(MEGABYTE(64));
    GameState->EntityManager.Init();

    // NOTE: Set timer variables
    GameState->Time = 0.0;
    GameState->DeltaTime = 0.0;

    // NOTE: Set pixel-meter conversion variables
    //GameState->MetersToPixels = Platform.WindowWidth / 32.0f; 
    GameState->MetersToPixels = Platform.WindowWidth / 30.0f; 
    GameState->PixelsToMeters = 1.0f / GameState->MetersToPixels;

    // NOTE: Game projection and view matrices
    //GameState->ProjMatrix = Orthographic(0.0f, 32.0f, 18.0f, 0.0f, -10.0f, 1.0f);
    GameState->ProjMatrix = Orthographic(0.0f, 30.0f, 16.875f, 0.0f, -10.0f, 1.0f);
    GameState->ViewMatrix = LookAt(vec3(0.0f), vec3i(0, 0, -1), vec3i(0, 1, 0));

    // NOTE: Virtual screen resolution 1280x720
    GameState->ScreenProjMatrix = Orthographic(0.0f, 1280.0f, 720.0f, 0.0f, -10.0f, 1.0f);
    GameState->ScreenViewMatrix = LookAt(vec3(0.0f), vec3i(0, 0, -1), vec3i(0, 1, 0));

    GameState->CameraP = vec2(0.0f);

    // NOTE: Make shaders
    GameState->SpriteShader = rndr::MakeShaderProgram("shader/sprite_vs.glsl", "shader/sprite_ps.glsl");
    GameState->TextShader = rndr::MakeShaderProgram("shader/sprite_vs.glsl", "shader/sprite_ps.glsl");

    // NOTE: Make Vertex buffers
    //GameState->EntityVertexBuffer = rndr::MakeBuffer(resource_type::VERTEX_BUFFER, MEGABYTE(32), true);
    GameState->SpriteRender = new render_cmd_list(MEGABYTE(2), GameState->SpriteShader, &GameState->ViewMatrix, &GameState->ProjMatrix);
    GameState->TextVertexBuffer = rndr::MakeBuffer(resource_type::VERTEX_BUFFER, MEGABYTE(5), true);
    GameState->TextRender = new render_cmd_list(MEGABYTE(1), GameState->TextShader, &GameState->ScreenViewMatrix, &GameState->ScreenProjMatrix);

    // NOTE: Make white texture
    bitmap WhiteBitmap;
    LoadBitmap(&WhiteBitmap, "sprite/white_texture.tga");
    GameState->WhiteTexture = rndr::MakeTexture(&WhiteBitmap, tex_param::TEX2D, tex_param::NEAREST, tex_param::CLAMP, false);

    //InitEditor(&EditorCtx);
    map_data VillageMap;
    VillageMap.Init("map/Village.tmx");

    GameState->Map.Init(&VillageMap);
    VillageMap.Shutdown();
    GameState->Map.GenerateStaticBuffer();

    GameState->CharacterTileset.Init("tileset/CharacterTileset.tsx");
    GameState->ItemTileset.Init("tileset/ItemTileset.tsx");

    void *M = GameState->StackAllocator.Allocate(sizeof(player_entity), 8);
    GameState->PlayerEntity = GameState->EntityManager.AddEntity<player_entity>(M);

    M = GameState->StackAllocator.Allocate(sizeof(follow_camera_entity), 8);
    GameState->EntityManager.AddEntity<follow_camera_entity>(M);
}

void GameUpdateAndRender(game_input *Input)
{
    static bool GamePaused = false;

    ImGui::Begin("Game Debug");
    ImGui::Checkbox("Pause Game?", &GamePaused);
    ImGui::End();

    // NOTE: Set game input pointer
    GameState->Input = Input;

    // NOTE: Update GameState timer when the game is not paused
    static r64 PrevGameStateTime = 0.0;
    if(!GamePaused)
    {
        GameState->Time += Input->DeltaTime;
        GameState->DeltaTime = GameState->Time - PrevGameStateTime;
        PrevGameStateTime = GameState->Time;

        // NOTE: Test GameState time with Input time
        //r32 InputFrameRate = 1 / Input->DeltaTime;
        //r32 InputMSFrame = 1000.0 * Input->DeltaTime;
        //r32 GameFrameRate = 1 / GameState->DeltaTime;
        //r32 GameMSFrame = 1000.0 * GameState->DeltaTime;
        //ASSERT(InputFrameRate == GameFrameRate);
        //ASSERT(InputMSFrame == GameMSFrame);
    }

    GameState->EntityManager.Update();
    GameState->EntityManager.Draw();
    
    GameState->Map.UpdateAndRender();

    GameState->CharacterTileset.Render();
    GameState->ItemTileset.Render();

    if(Input->Mouse.Left.EndedDown)
    {
        GameState->CameraP.x -= Input->Mouse.xrel * GameState->PixelsToMeters * 1.0f;
        GameState->CameraP.y -= Input->Mouse.yrel * GameState->PixelsToMeters * 1.0f;
    }

    GameState->ViewMatrix = LookAt(vec3(GameState->CameraP, 0.0f), vec3(GameState->CameraP, 0.0f) + vec3i(0, 0, -1), vec3i(0, 1, 0));
    
    // NOTE: FPS Counter
    DebugDraw->Text(Rect(0, 720, 1, 1), RGBAUnpackTo01(0xa3f736ff), 10.0f, "%0.2f ms/frame Framerate %0.2f/s", 1000.0 * Input->DeltaTime, 1 / Input->DeltaTime);
    //DebugDraw->Text(Rect(400, 720, 1, 1), RGBAUnpackTo01(0xa3f736ff), 10.0f, "GameTime: %0.6f GameDeltaTime: %0.6f", GameState->Time, GameState->DeltaTime);
    
    rndr::Clear(); // NOTE: Clear all framebuffers

    // NOTE: Sprite rendering
    GameState->SpriteRender->Sort();
    GameState->SpriteRender->Submit();
    GameState->SpriteRender->Flush();

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