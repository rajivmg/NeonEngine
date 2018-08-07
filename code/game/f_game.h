#ifndef F_GAME_H
#define F_GAME_H

#include <core/neon_platform.h>
#include <core/neon_bitmap.h>
#include <core/neon_font.h>
#include <vector>

enum game_mode
{
    GameMode_Editor,
    GameMode_Play,
    GameMode_Pause
};

struct game_tile
{
    u16 ID;
    vec4 UV;
};

struct game_level
{
    game_tile Level[20][12];
};

enum game_object_type
{
    GameObject_Null,
    GameObject_Wall,
    GameObject_Crate,
    GameObject_Player,
    GameObject_Floor
};

struct game_object
{
    game_object_type Type;
    s32 X, Y;
    bool Moving;

    // Rendering info
    vec2 P; // x, y coords for rendering
    r32 UV0, UV1, UV2, UV3; // BLx, BLy, TRx, TRy
};

struct level
{
    game_object Objects[8][8][2];
    vec2 Player;
    bool EntityMoving;
};
#define GET_ENTITY(RoomPtr, X, Y, Z) ((RoomPtr)->Objects[(s32)(Y)][(s32)(X)][(s32)(Z)])

struct game_state
{
    game_mode GameMode;

    r32 MetersToPixels;
    r32 PixelsToMeters;

    render_resource WhiteTexture;
    render_resource SpriteShader;
    render_resource TextShader;

    font *DbgFont;
    render_resource DbgTextVertexBuffer;
    std::vector<vert_P1C1UV1> DbgTextVertices;
    render_cmd_list *DbgTextRender;

    render_resource DbgLineShader;
    render_resource DbgLineVertexBuffer;
    std::vector<vert_P1C1> DbgLineVertices;
    render_cmd_list *DbgLineRender;

    render_resource AtlasTexture;
    game_level Level;
    
    render_cmd_list *GameRender;
    render_resource GameVertexBuffer;
    std::vector<vert_P1C1UV1> GameVertices;
};
#endif