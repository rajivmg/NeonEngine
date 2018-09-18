#include "game.h"

#include "editor.h"
#include <dear-imgui/imgui.h>
#include "debug_draw.h"

#define CUTE_C2_IMPLEMENTATION
#include "cute_c2.h"

debug_draw *DebugDraw = nullptr; // NOTE: Global DebugDraw object pointer

static game_state GameState = {};
static editor_state EditorCtx = {};

void DrawText(rect Dest, vec4 Color, r32 Layer, font *Font, char const *Format, ...)
{
    u32 StartV, CountV;
    StartV = (u32)GameState.TextVertices.size();

    va_list ArgList;
    va_start(ArgList, Format);
    PushTextV(&GameState.TextVertices, Dest, Color, Layer, Font, Format, ArgList);
    va_end(ArgList);

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
    rndr::Init(); // NOTE: Initialize renderer

    DebugDraw = new debug_draw(); // NOTE: Construct global DebugDraw object

    // TODO: Remove this
    GameState.MetersToPixels = Platform.WindowWidth / 16.0f; 
    GameState.PixelsToMeters = 1.0f / GameState.MetersToPixels;

    // NOTE: Game projection and view matrices
    GameState.ProjMatrix = Orthographic(0.0f, 16.0f, 9.0f, 0.0f, -10.0f, 1.0f);
    GameState.ViewMatrix = LookAt(vec3(0.0f), vec3i(0, 0, -1), vec3i(0, 1, 0));

    // NOTE: Virtual screen resolution 1280x720
    GameState.ScreenProjMatrix = Orthographic(0.0f, 1280.0f, 720.0f, 0.0f, -10.0f, 1.0f);
    GameState.ScreenViewMatrix = LookAt(vec3(0.0f), vec3i(0, 0, -1), vec3i(0, 1, 0));

    // NOTE: Make shaders
    GameState.SpriteShader = rndr::MakeShaderProgram("shaders/sprite_vs.glsl", "shaders/sprite_ps.glsl");
    GameState.TextShader = rndr::MakeShaderProgram("shaders/sprite_vs.glsl", "shaders/sprite_ps.glsl");

    // NOTE: Make Vertex buffers
    GameState.SpriteVertexBuffer = rndr::MakeBuffer(resource_type::VERTEX_BUFFER, MEGABYTE(20), true);
    GameState.SpriteRender = new render_cmd_list(MEGABYTE(2), GameState.SpriteShader, &GameState.ViewMatrix, &GameState.ProjMatrix);
    GameState.TextVertexBuffer = rndr::MakeBuffer(resource_type::VERTEX_BUFFER, MEGABYTE(5), true);
    GameState.TextRender = new render_cmd_list(MEGABYTE(1), GameState.TextShader, &GameState.ScreenViewMatrix, &GameState.ScreenProjMatrix);

    bitmap AtlasBitmap;
    LoadBitmap(&AtlasBitmap, "sprites/T.tga");
    GameState.AtlasTexture = rndr::MakeTexture(&AtlasBitmap, tex_param::TEX2D, tex_param::NEAREST, tex_param::CLAMP, true);

    bitmap WhiteBitmap;
    LoadBitmap(&WhiteBitmap, "sprites/white_texture.tga");
    GameState.WhiteTexture = rndr::MakeTexture(&WhiteBitmap, tex_param::TEX2D, tex_param::NEAREST, tex_param::CLAMP, false);

    InitEditor(&EditorCtx);
}

void GameUpdateAndRender(game_input *Input)
{
    // NOTE: Load assets
    static game_assets *Assets = new game_assets();

    // NOTE: Setup World
    static world *World = nullptr;
    if(!World)
    {
        World = (world *)MALLOC(sizeof(world));
        GenChunk(World);
        World->GuyP = vec2(8.0f, 4.0f);
        World->GuyJumping = false;
        World->GuyFootSpeed = 1.0f;

        const r32 PeekHeight = 1.2f;
        const r32 PeekHeightHoriDist = 0.5f;
        World->JumpVelocity = (2.0f * PeekHeight * World->GuyFootSpeed) / PeekHeightHoriDist;
        World->Gravity = (-2.0f * PeekHeight * World->GuyFootSpeed * World->GuyFootSpeed) / (PeekHeightHoriDist * PeekHeightHoriDist);
    }

    WorldSimulate(Input, World);

    // NOTE: Generate world render commands
    // TODO: Currently tile _MAY_ have pixel wide gap between them, investigate.
    for(u32 YCoord = 0; YCoord < ARRAY_COUNT(world_chunk::Tiles[0]); ++YCoord)
    {
        for(u32 XCoord = 0; XCoord < ARRAY_COUNT(world_chunk::Tiles); ++XCoord)
        {
            if(World->Chunk.Tiles[XCoord][YCoord])
            {
                game_tile *Tile = Assets->GetTileByID(World->Chunk.Tiles[XCoord][YCoord]);
                PushSprite(&GameState.SpriteVertices, Rect((r32)XCoord, (r32)YCoord, 1.0f, 1.0f), Tile->UV, vec4(1.0f), 0.0f, vec2(0.0f), 1.0f);
            }
        }
    }

    // NOTE: Player Sprite
    PushSprite(&GameState.SpriteVertices, Rect(World->GuyP.x, World->GuyP.y, 1, 1), Assets->GetTileByID(14)->UV, vec4(1.0f), 0.0f, vec2(0.0f), 2.0f);
 
    // NOTE: FPS Counter
    DebugDraw->Text(Rect(0, 720, 1, 1), RGBAUnpackTo01(0xa3f736ff), 10.0f, "%0.2f ms/frame Framerate %0.2f/s", 1000.0 * Input->DeltaTime, 1 / Input->DeltaTime);

#if 0
    vec4 NormalColor = RGBAUnpackTo01(0xFF7799FF);
    vec4 CollisionColor = RGBAUnpackTo01(0xFF0066FF);

    static vec2 AP = vec2(320.0f, 0.0f), BP = vec2(100.0f, 280.0f); // NOTE: AP is moving, BP is static
    static vec2 APSize = vec2(100.0f, 100.0f), BPSize(100.0f, 100.0f);
    static vec2 LastAP, LastBP;
    r32 TFirst = 0.0f, TLast = 0.0f;

    //static vec2 AP(Input->Mouse.x - (APSize.x / 2.0f), Input->Mouse.y - (APSize.y / 2.0f));
    bool IsColliding = false;
    //static vec2 MovingVelocity = vec2(0.0f, 100.0f);
    //if(AP.y >= Platform.WindowHeight - APSize.y || AP.y <= 0.0f)
    //{
    //    MovingVelocity *= vec2(1.0f, -1.0f);
    //}

    vec2 MovingVelocity = vec2(0.0f, 0.0f);
    if(Input->Controllers[0].Left.EndedDown) MovingVelocity = vec2(-150.0f, 0.0f);
    if(Input->Controllers[0].Right.EndedDown) MovingVelocity = vec2(150.0f, 0.0f);
    if(Input->Controllers[0].Up.EndedDown) MovingVelocity = vec2(0.0f, 150.0f);
    if(Input->Controllers[0].Down.EndedDown) MovingVelocity = vec2(0.0f, -150.0f);

    AP += MovingVelocity * (r32)Input->DeltaTime;

    vec2 VA = AP - LastAP; 
    vec2 VB = BP - LastBP;

    c2AABB A, B;
    A.min = c2V(AP.x, AP.y);
    A.max = c2V(AP.x + APSize.x, AP.y + APSize.y);
    B.min = c2V(BP.x, BP.y);
    B.max = c2V(BP.x + BPSize.x, BP.y + BPSize.y);

    c2Manifold ABManifold;
    c2AABBtoAABBManifold(A, B, &ABManifold);
    if(ABManifold.count != 0)
    {
        IsColliding = true;
        AP.x -= ABManifold.n.x * ABManifold.depths[0] + 0.1f * ABManifold.n.x;
        AP.y -= ABManifold.n.y * ABManifold.depths[0] + 0.1f * ABManifold.n.y;
    }
    //if(VA.x != 0.0f || VA.y != 0.0f || VB.x != 0.0f || VB.y != 0.0f)
    //IsColliding = IntersectMovingAABBAABB(AABBMinDim(AP, APSize), AABBMinDim(BP, BPSize), VA, VB, TFirst, TLast);
    //Platform.Log("TFirst=%f TLast=%f", TFirst, TLast);
    //if(IsColliding)
    //{
    //    AP -= (1.0f - TFirst) * MovingVelocity * (r32)Input->DeltaTime;
    //}
    if(IsColliding)
        PushDbgRect(&GameState.DbgRectVertices, Rect(ABManifold.contact_points[0].x-2.5f, ABManifold.contact_points[0].y-2.5f, 5, 5), vec4i(0, 1, 0, 1), 3.0f);
    PushDbgRect(&GameState.DbgRectVertices, Rect(AP.x, AP.y, APSize.x, APSize.y), RGBAUnpackTo01(0xFFAA99FF), 3.0f); // NOTE: Draw AP
    PushDbgRect(&GameState.DbgRectVertices, Rect(BP.x, BP.y, BPSize.x, BPSize.y), IsColliding ? CollisionColor : NormalColor, 3.0f); // NOTE: Draw BP
    PushText(&GameState.DbgTextVertices, Rect(500, 200, 1, 1), vec4i(1, 1, 0, 1), 10.0f, &GameState.DbgFont, "TFirst = %0.4f TLast = %0.4f\nIsColliding = %d", TFirst, TLast, IsColliding);
    LastAP = AP; LastBP = BP;
#endif 
    rndr::Clear(); // NOTE: Clear all framebuffers

    //EditorUpdateAndRender(&EditorCtx, Input);
 
    // NOTE: Sprite rendering commands
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
    
    // NOTE: Text rendering commands
    if(!GameState.TextVertices.empty())
    {
        rndr::BufferData(GameState.TextVertexBuffer, 0, (u32)sizeof(vert_P1C1UV1) * (u32)GameState.TextVertices.size(), &GameState.TextVertices.front());
        GameState.TextVertices.clear();
        GameState.TextRender->Sort();
        GameState.TextRender->Submit();
        GameState.TextRender->Flush();
    }

    // NOTE: Debug Draw commands
    DebugDraw->Submit();
}