#include "neon_game.h"

#include "neon_mesh.h"

static game_state GameState = {};

void GenWorld()
{
    GameState.DbgLineVertices.clear();
    GameState.WorldVertices.clear();
    ImGui::Begin("Sprite Test");
    local_persist r32 AngleRad = 0.0f;
    local_persist vec2 RectP = vec2i(2, 2);
    local_persist vec2 RectO = vec2i(0, 0);
    local_persist r32 Scale = 1.0f;
    ImGui::SliderAngle("Rotation Angle", &AngleRad);
    ImGui::SliderFloat("Scale", &Scale, 0.1f, 5.0f);
    ImGui::InputFloat2("Origin", RectO.Elements);
    ImGui::End();
    PushSprite(&GameState.WorldVertices, Rect(RectP.x, RectP.y, 4, 4), vec4i(0, 0, 1, 1), vec4i(1, 1, 1, 1), AngleRad, RectO, vec2(Scale), 0.0f);
    rndr::BufferData(GameState.WorldVertexBuffer, 0, (u32)sizeof(vert_P1C1UV1) * (u32)GameState.WorldVertices.size(), &GameState.WorldVertices.front());
    PushDbgLine(&GameState.DbgLineVertices, vec3(RectP.x + RectO.x, RectP.y + RectO.y, 1.0f), vec3(100.0f, RectP.y + RectO.y, 1.0f), vec4i(1, 1, 0, 1));
    PushDbgLine(&GameState.DbgLineVertices, vec3(RectP.x + RectO.x, RectP.y + RectO.y, 1.0f), vec3(RectP.x + RectO.x, 100.0f, 1.0f), vec4i(1, 1, 0, 1));
}

DLLEXPORT
GAME_SETUP(GameSetup)
{
    // Set pointers and init renderer
    Platform = _Platform;
    ImGui::SetCurrentContext(_ImGuiCtx);
    rndr::Init();

    // GameState
    GameState.MetersToPixels = Platform.WindowWidth / 16.0f;
    GameState.PixelsToMeters = 1.0f / GameState.MetersToPixels;

    // Screen is 16x9 meters
    mat4 ViewMatrix = LookAt(vec3(0.0f), vec3(0.0f, 0.0f, -1.0f), vec3(0.0f, 1.0f, 0.0f));
    mat4 ProjMatrix = Orthographic(0.0f, 16.0f, 9.0f, 0.0f, -1.0f, 1.0f);

    InitFont(&GameState.DbgFont, "fonts/Inconsolata/Inconsolata-Regular.ttf", 20);

    bitmap WonderArtBitmap;
    LoadBitmap(&WonderArtBitmap, "Wonder_Art.tga");
    GameState.WonderArtTexture = rndr::MakeTexture(&WonderArtBitmap, texture_type::TEXTURE_2D, texture_filter::NEAREST, texture_wrap::CLAMP, true);

    bitmap WhiteBitmap;
    LoadBitmap(&WhiteBitmap, "sprites/white_texture.tga");
    GameState.WhiteTexture = rndr::MakeTexture(&WhiteBitmap, texture_type::TEXTURE_2D, texture_filter::NEAREST, texture_wrap::CLAMP, false);

    GameState.SpriteShader = rndr::MakeShaderProgram("shaders/sprite_vs.glsl", "shaders/sprite_ps.glsl");

    GameState.TextShader = rndr::MakeShaderProgram("shaders/sprite_vs.glsl", "shaders/sprite_ps.glsl");
    GameState.DbgTextVertexBuffer = rndr::MakeBuffer(resource_type::VERTEX_BUFFER, MEGABYTE(1), true);
    GameState.DbgTextRender = new render_cmd_list(MEGABYTE(1), GameState.TextShader);
    GameState.DbgTextRender->ViewMatrix = LookAt(vec3(0.0f), vec3i(0, 0, -1), vec3i(0, 1, 0));
    GameState.DbgTextRender->ProjMatrix = Orthographic(0.0f, (r32)Platform.WindowWidth, (r32)Platform.WindowHeight, 0.0f, -1.0f, 1.0f);

    GameState.DbgLineShader = rndr::MakeShaderProgram("shaders/debug_line_vs.glsl", "shaders/debug_line_ps.glsl");
    GameState.DbgLineVertexBuffer = rndr::MakeBuffer(resource_type::VERTEX_BUFFER, MEGABYTE(5), true);
    GameState.DbgLineRender = new render_cmd_list(MEGABYTE(1), GameState.DbgLineShader);
    GameState.DbgLineRender->ViewMatrix = ViewMatrix;
    GameState.DbgLineRender->ProjMatrix = ProjMatrix;

    GameState.WorldRender = new render_cmd_list(MEGABYTE(2), GameState.SpriteShader);
    GameState.WorldRender->ViewMatrix = ViewMatrix;
    GameState.WorldRender->ProjMatrix = ProjMatrix;

    GameState.WorldVertexBuffer = rndr::MakeBuffer(resource_type::VERTEX_BUFFER, MEGABYTE(20), true);

    //GenWorld();
    //vec4 Color = RGBAUnpack4x8(0x0C0C0CFF);
    int a = 0;
}

DLLEXPORT
GAME_UPDATE_AND_RENDER(GameUpdateAndRender)
{
    //-----------------------------------------------------------------------------
    // Game Update
    //-----------------------------------------------------------------------------

    GenWorld();
    //-----------------------------------------------------------------------------
    // Game Render
    //-----------------------------------------------------------------------------
    rndr::Clear();

    ImGui::Begin("Debug");
    ImGui::Text("%0.2f ms/frame", 1000.0f * Input->DeltaTime);
    ImGui::End();

    cmd::draw *WorldDrawCmd = GameState.WorldRender->AddCommand<cmd::draw>(0);
    WorldDrawCmd->VertexBuffer = GameState.WorldVertexBuffer;
    WorldDrawCmd->VertexFormat = vert_format::P1C1UV1;
    WorldDrawCmd->StartVertex = 0;
    WorldDrawCmd->VertexCount = (u32)GameState.WorldVertices.size();
    WorldDrawCmd->Textures[0] = GameState.WonderArtTexture;

    GameState.WorldRender->Sort();
    GameState.WorldRender->Submit();
    GameState.WorldRender->Flush();

    std::vector<vert_P1C1UV1> DebugTextVertices;

    //PushSprite(&TilesVertices, vec3i(0, 0, 0), vec2i(7, 7), vec4(0.5f, 0, 0, 1), vec4i(0, 0, 1, 1));

    PushText(&DebugTextVertices, &GameState.DbgFont, vec3i(0, 720, 1), vec4i(1, 1, 0, 1), "%0.2f ms/frame Framerate %ff/s", 1000.0 * Input->DeltaTime, 1 / Input->DeltaTime);
    PushText(&DebugTextVertices, &GameState.DbgFont, vec3i(400, 720, 1), vec4i(1, 1, 0, 1), "Time %f", Input->Time);
    rndr::BufferData(GameState.DbgTextVertexBuffer, 0, (u32)sizeof(vert_P1C1UV1) * (u32)DebugTextVertices.size(), &DebugTextVertices.front());

    cmd::draw *DebugTextCmd = GameState.DbgTextRender->AddCommand<cmd::draw>(0);
    DebugTextCmd->VertexBuffer = GameState.DbgTextVertexBuffer;
    DebugTextCmd->VertexFormat = vert_format::P1C1UV1;
    DebugTextCmd->StartVertex = 0;
    DebugTextCmd->VertexCount = (u32)DebugTextVertices.size();
    DebugTextCmd->Textures[0] = GameState.DbgFont.Texture;

    GameState.DbgTextRender->Sort();
    GameState.DbgTextRender->Submit();
    GameState.DbgTextRender->Flush();

#if 1
    //std::vector<vert_P1C1> DebugLinesVertices;
    //PushLine(&GameState.DbgLineVertices, vec3(2, 2, 0), vec3(12, 7, 0), vec4(1, 0, 1, 1));
    //u32 StepAngle = 4;
    //r32 Radius = 2.2f;
    //for(u32 I = 0; I < 360; I += StepAngle)
    //{
    //    vec3 FromP = vec3(8.0f, 4.5f, 0) + vec3(cosf(DEG2RAD(I)), sinf(DEG2RAD(I)), 0.0f) * Radius;
    //    vec3 ToP = vec3(8.0f, 4.5f, 0) + vec3(cosf(DEG2RAD(I + StepAngle)), sinf(DEG2RAD(I + StepAngle)), 0.0f) * Radius;
    //    PushDbgLine(&GameState.DbgLineVertices, FromP, ToP, RGBA255To01(RGBAUnpack4x8(0xFF00FFFF)));
    //}

    rndr::BufferData(GameState.DbgLineVertexBuffer, 0, (u32)sizeof(vert_P1C1) * (u32)GameState.DbgLineVertices.size(), &GameState.DbgLineVertices.front());

    cmd::draw_debug_lines *DrawDebugLinesCmd = GameState.DbgLineRender->AddCommand<cmd::draw_debug_lines>(0);
    DrawDebugLinesCmd->VertexBuffer = GameState.DbgLineVertexBuffer;
    DrawDebugLinesCmd->VertexFormat = vert_format::P1C1;
    DrawDebugLinesCmd->StartVertex = 0;
    DrawDebugLinesCmd->VertexCount = (u32)GameState.DbgLineVertices.size();

    GameState.DbgLineRender->Sort();
    GameState.DbgLineRender->Submit();
    GameState.DbgLineRender->Flush();
#endif
}