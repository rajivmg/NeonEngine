#include "neon_game.h"
#include <dear-imgui/imgui.h>

static game_state GameState = {};

void GameSetup()
{
    // Set pointers and init renderer
    rndr::Init();

    // GameState
    GameState.MetersToPixels = Platform.WindowWidth / 16.0f;
    GameState.PixelsToMeters = 1.0f / GameState.MetersToPixels;

    // Screen is 16x9 meters
    mat4 ViewMatrix = LookAt(vec3(0.0f), vec3(0.0f, 0.0f, -1.0f), vec3(0.0f, 1.0f, 0.0f));
    mat4 ProjMatrix = Orthographic(0.0f, 16.0f, 9.0f, 0.0f, -1.0f, 1.0f);

    GameState.DbgFont = new font("fonts/dbg_font_26.fnt");

    bitmap WonderArtBitmap;
    LoadBitmap(&WonderArtBitmap, "sprites/cavesofgallet_tiles.tga");

    GameState.EditorTilesetTexture = rndr::MakeTexture(&WonderArtBitmap, tex_param::TEX2D, tex_param::NEAREST, tex_param::CLAMP, false);
    GameState.GameTilesetTexture = rndr::MakeTexture(&WonderArtBitmap, tex_param::TEX2D, tex_param::NEAREST, tex_param::CLAMP, true);
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

    GameState.WorldRender = new render_cmd_list(MEGABYTE(2), GameState.SpriteShader);
    GameState.WorldRender->ViewMatrix = ViewMatrix;
    GameState.WorldRender->ProjMatrix = ProjMatrix;

    GameState.WorldVertexBuffer = rndr::MakeBuffer(resource_type::VERTEX_BUFFER, MEGABYTE(20), true);

    //vec4 Color = RGBAUnpack4x8(0x0C0C0CFF);
    int a = 0;
}

void GameUpdateAndRender(game_input *Input)
{
    //-----------------------------------------------------------------------------
    // Game Update
    //-----------------------------------------------------------------------------
    EditorTick(&GameState);

    //-----------------------------------------------------------------------------
    // Game Render
    //-----------------------------------------------------------------------------
    rndr::Clear();

    //ImGui::Begin("Debug");
    //ImGui::Text("%0.2f ms/frame", 1000.0f * Input->DeltaTime);
    //ImGui::End();

    cmd::draw *WorldDrawCmd = GameState.WorldRender->AddCommand<cmd::draw>(0);
    WorldDrawCmd->VertexBuffer = GameState.WorldVertexBuffer;
    WorldDrawCmd->VertexFormat = vert_format::P1C1UV1;
    WorldDrawCmd->StartVertex = 0;
    WorldDrawCmd->VertexCount = (u32)GameState.WorldVertices.size();
    WorldDrawCmd->Textures[0] = GameState.GameTilesetTexture;

    GameState.WorldRender->Sort();
    GameState.WorldRender->Submit();
    GameState.WorldRender->Flush();

    std::vector<vert_P1C1UV1> DebugTextVertices;

    PushText(&DebugTextVertices, Rect(600, 720, 1, 1), vec4i(1, 1, 0, 1), 1.0f, GameState.DbgFont, "A quick brown fox: %d\nA quick brown fox: %d\nJoe\nNEON", 1, 2);
    PushText(&DebugTextVertices, Rect(0, 720, 1, 1), vec4i(1, 1, 0, 1), 1.0f, GameState.DbgFont, "%0.2f ms/frame Framerate %ff/s", 1000.0 * Input->DeltaTime, 1 / Input->DeltaTime);
    PushText(&DebugTextVertices, Rect(400, 720, 1, 1), vec4i(1, 1, 0, 1), 1.0f, GameState.DbgFont, "Time %f", Input->Time);
    rndr::BufferData(GameState.DbgTextVertexBuffer, 0, (u32)sizeof(vert_P1C1UV1) * (u32)DebugTextVertices.size(), &DebugTextVertices.front());

    cmd::draw *DebugTextCmd = GameState.DbgTextRender->AddCommand<cmd::draw>(0);
    DebugTextCmd->VertexBuffer = GameState.DbgTextVertexBuffer;
    DebugTextCmd->VertexFormat = vert_format::P1C1UV1;
    DebugTextCmd->StartVertex = 0;
    DebugTextCmd->VertexCount = (u32)DebugTextVertices.size();
    DebugTextCmd->Textures[0] = GameState.DbgFont->Texture;

    GameState.DbgTextRender->Sort();
    GameState.DbgTextRender->Submit();
    GameState.DbgTextRender->Flush();

#if 0
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

    ne::BufferData(GameState.DbgLineVertexBuffer, 0, (u32)sizeof(vert_P1C1) * (u32)GameState.DbgLineVertices.size(), &GameState.DbgLineVertices.front());

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