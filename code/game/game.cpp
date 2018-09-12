#include "game.h"

#include "editor.h"
#include <dear-imgui/imgui.h>
#include <rapidxml/rapidxml.hpp>
#include <rapidxml/rapidxml_print.hpp>
#include <Box2D/Box2D.h>
using namespace rapidxml;


static game_state GameState = {};
static editor_state EditorCtx = {};

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
    rndr::Init(); // NOTE: Initialize renderer

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
    GameState.DbgLineShader = rndr::MakeShaderProgram("shaders/debug_line_vs.glsl", "shaders/debug_line_ps.glsl");
    GameState.DbgRectShader = rndr::MakeShaderProgram("shaders/debug_rect_vs.glsl", "shaders/debug_rect_ps.glsl");

    // NOTE: Make Vertex buffers
    GameState.DbgTextVertexBuffer = rndr::MakeBuffer(resource_type::VERTEX_BUFFER, MEGABYTE(1), true);
    GameState.DbgTextRender = new render_cmd_list(MEGABYTE(1), GameState.TextShader, &GameState.ScreenViewMatrix, &GameState.ScreenProjMatrix);
    GameState.DbgLineVertexBuffer = rndr::MakeBuffer(resource_type::VERTEX_BUFFER, MEGABYTE(5), true);
    GameState.DbgLineRender = new render_cmd_list(MEGABYTE(1), GameState.DbgLineShader, &GameState.ScreenViewMatrix, &GameState.ScreenProjMatrix);
    GameState.DbgRectVertexBuffer = rndr::MakeBuffer(resource_type::VERTEX_BUFFER, MEGABYTE(1), true);
    GameState.DbgRectRender = new render_cmd_list(MEGABYTE(1), GameState.DbgRectShader, &GameState.ScreenViewMatrix, &GameState.ScreenProjMatrix);

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

    InitFont(&GameState.DbgFont, "fonts/inconsolata_26.fnt");
    InitFont(&GameState.MainFont, "fonts/zorque_42.fnt");

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
    }

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

    static vec2 PlayerWorldP = vec2(8.0f, 4.0f);
#if 0
    u32 GridCol = (u32)floorf(PlayerWorldP.x / 1.0f);
    u32 GridRow = (u32)floorf(PlayerWorldP.y / 1.0f);

    vec2 PlayerdP = PlayerWorldP;
    if(Input->Controllers[0].Left.EndedDown)
    {
        PlayerdP.x += -1.5f * Input->DeltaTime;
    }
    else if(Input->Controllers[0].Right.EndedDown)
    {
        PlayerdP.x += 1.5f * Input->DeltaTime;
    }

    bool Colliding = false;
    for(u32 YCoord = 0; YCoord < ARRAY_COUNT(world_chunk::Tiles[0]); ++YCoord)
    {
        for(u32 XCoord = 0; XCoord < ARRAY_COUNT(world_chunk::Tiles); ++XCoord)
        {
            if(World->Chunk.Tiles[XCoord][YCoord])
            {
                if(IntersectAABBAABB(AABBMinMax(PlayerdP + 0.20f, PlayerdP + 0.20f + vec2(0.5f)), AABBMinMax(vec2(XCoord, YCoord), vec2(XCoord, YCoord) + vec2(1.0f))))
                {
                    Colliding = true;
                }
            }
        }
    }

    if(!Colliding)
    {
        PlayerWorldP = PlayerdP;
    }
#endif

    // NOTE: Player Sprite
    PushSprite(&GameState.SpriteVertices, Rect(PlayerWorldP.x, PlayerWorldP.y, 1, 1), Assets->GetTileByID(13)->UV, vec4(1.0f), 0.0f, vec2(0.0f), 2.0f);
 
    // NOTE: FPS Counter
    PushText(&GameState.DbgTextVertices, Rect(0, 720, 1, 1), vec4i(1, 0, 1, 1), 10.0f, &GameState.DbgFont, "%0.2f ms/frame Framerate %ff/s", 1000.0 * Input->DeltaTime, 1 / Input->DeltaTime);

#if 0
    // NOTE: Collision Test Simulation
    static b2World *PhyWorld = nullptr;
    static b2Body *GroundBody;
    static b2Body *BoxBody;
    if(PhyWorld == nullptr)
    {
        b2Vec2 PhyGravity(0.0f, -7.0f);
        PhyWorld = new b2World(PhyGravity);

        b2BodyDef GroundBodyDef;
        GroundBodyDef.type = b2_staticBody;
        GroundBodyDef.position.Set(5.0f, 2.1f);
        GroundBody = PhyWorld->CreateBody(&GroundBodyDef);
        b2PolygonShape GroundBox;
        GroundBox.SetAsBox(5.0f, 1.0f);
        GroundBody->CreateFixture(&GroundBox, 0.0f);

        b2BodyDef BoxBodyDef;
        BoxBodyDef.type = b2_dynamicBody;
        BoxBodyDef.position.Set(5.0f, 8.0f);
        BoxBody = PhyWorld->CreateBody(&BoxBodyDef);
        b2PolygonShape Box;
        Box.SetAsBox(1.0f, 1.0f);
        b2FixtureDef BoxFixtureDef;
        BoxFixtureDef.shape = &Box;
        BoxFixtureDef.density = 1.0f;
        BoxFixtureDef.friction = 0.3f;
        BoxFixtureDef.restitution = 0.4f;
        BoxBody->CreateFixture(&BoxFixtureDef);
    }

    PhyWorld->Step(1.0f / 60.0f, 8, 3);

    b2Vec2 GroundP = GroundBody->GetPosition();
    b2Vec2 BoxP = BoxBody->GetPosition();
    PushSprite(&GameState.SpriteVertices, Rect(GroundP.x - 5.0f, GroundP.y - 1.0f, 10.0f, 2.0f), Assets->GetTileByID(12)->UV, vec4i(1, 1, 1, 1), 0.0f, vec2(0.0f), 3.0f);
    PushSprite(&GameState.SpriteVertices, Rect(BoxP.x - 1.0f, BoxP.y - 1.0f, 1.0f, 1.0f), Assets->GetTileByID(13)->UV, vec4i(1, 1, 1, 1), 0.0f, vec2(0.0f), 3.0f);
#endif

#if 1
    vec4 NormalColor = RGBAUnpackTo01(0xFF7799FF);
    vec4 CollisionColor = RGBAUnpackTo01(0xFF0066FF);

    static vec2 AP = vec2(420.0f, 0.0f), BP = vec2(100.0f, 280.0f); // NOTE: AP is moving, BP is static
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
    if(Input->Controllers[0].Left.EndedDown) MovingVelocity = vec2(-50.0f, 0.0f);
    if(Input->Controllers[0].Right.EndedDown) MovingVelocity = vec2(50.0f, 0.0f);
    if(Input->Controllers[0].Up.EndedDown) MovingVelocity = vec2(0.0f, 50.0f);
    if(Input->Controllers[0].Down.EndedDown) MovingVelocity = vec2(0.0f, -50.0f);

    AP += MovingVelocity * (r32)Input->DeltaTime;

    vec2 VA = AP - LastAP; 
    vec2 VB = BP - LastBP;

    if(VA.x != 0.0f || VA.y != 0.0f || VB.x != 0.0f || VB.y != 0.0f)
    IsColliding = IntersectMovingAABBAABB(AABBMinDim(AP, APSize), AABBMinDim(BP, BPSize), VA, VB, TFirst, TLast);
    Platform.Log("TFirst=%f TLast=%f", TFirst, TLast);
    if(IsColliding)
    {
        AP -= (1.0f - TFirst) * MovingVelocity * (r32)Input->DeltaTime;
        //MovingVelocity.x = 100.0f;
        //MovingVelocity.y = 0.0f;
    }

    PushDbgRect(&GameState.DbgRectVertices, Rect(AP.x, AP.y, APSize.x, APSize.y), RGBAUnpackTo01(0xFFAA99FF), 3.0f); // NOTE: Draw AP
    PushDbgRect(&GameState.DbgRectVertices, Rect(BP.x, BP.y, BPSize.x, BPSize.y), IsColliding ? CollisionColor : NormalColor, 3.0f); // NOTE: Draw BP
    PushText(&GameState.DbgTextVertices, Rect(500, 200, 1, 1), vec4i(1, 1, 0, 1), 10.0f, &GameState.DbgFont, "TFirst = %0.4f TLast = %0.4f\nIsColliding = %d", TFirst, TLast, IsColliding);
    LastAP = AP; LastBP = BP;
#endif
    /*
    vec3 Dir = Normalize(vec3(RectA.x - RectALast.x, RectA.y - RectALast.y, 1.0f)) * Input->DeltaTime; // Normalize(vec3i(Input->Mouse.xrel, Input->Mouse.yrel, 1.0f));
    bool IsIntersecting = IntersectMovingAABBAABB(AABBRect(RectB), AABBRect(RectA), Dir, vec3(0.0f), TFirst, TLast);
    RectALast = RectA;

    PushDbgRect(&GameState.DbgRectVertices, RectA, RGBAUnpackTo01(0xFFAA99FF), 2.0f);
    PushDbgRect(&GameState.DbgRectVertices, RectB, IsIntersecting ? IntersColor : NormalColor, 2.0f);
    PushText(&GameState.DbgTextVertices, Rect(700, 720, 1, 1), vec4i(1, 1, 0, 1), 10.0f, &GameState.DbgFont, "TFirst = %f TLast = %f", TFirst, TLast);
    */
    //PushDbgLine(&GameState.DbgLineVertices, vec3i(640, 360, 4.0f), vec3(Dir.x * 200, Dir.y * 200, 4.0f) + vec3i(640, 360, 4.0f), vec4i(1, 0, 0, 1));
    
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
    // NOTE: Debug text rendering command
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
    // NOTE: Debug line rendering command
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
    if(!GameState.DbgRectVertices.empty())
    {
        rndr::BufferData(GameState.DbgRectVertexBuffer, 0, (u32)GameState.DbgRectVertices.size() * (u32)sizeof(vert_P1C1), &GameState.DbgRectVertices.front());
        
        cmd::draw *DrawDebugRectCmd = GameState.DbgRectRender->AddCommand<cmd::draw>(0);
        DrawDebugRectCmd->VertexBuffer = GameState.DbgRectVertexBuffer;
        DrawDebugRectCmd->VertexFormat = vert_format::P1C1;
        DrawDebugRectCmd->StartVertex = 0;
        DrawDebugRectCmd->VertexCount = (u32)GameState.DbgRectVertices.size();

        GameState.DbgRectVertices.clear();

        GameState.DbgRectRender->Sort();
        GameState.DbgRectRender->Submit();
        GameState.DbgRectRender->Flush();
    }
}