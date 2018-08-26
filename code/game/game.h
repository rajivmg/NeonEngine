#ifndef GAME_H
#define GAME_H

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
    game_tile Level[50][50];
    u32 SizeX, SizeY;
};

struct game_state
{
    game_mode GameMode;

    r32 MetersToPixels;
    r32 PixelsToMeters;

    mat4 ScreenProjMatrix;
    mat4 ScreenViewMatrix;
    mat4 GameProjMatrix;
    mat4 GameViewMatrix;

    render_resource WhiteTexture;
    render_resource SpriteShader;
    render_resource TextShader;

    font MainFont;
    font DbgFont;

    render_resource DbgTextVertexBuffer;
    std::vector<vert_P1C1UV1> DbgTextVertices;
    render_cmd_list *DbgTextRender;

    render_resource DbgLineShader;
    render_resource DbgLineVertexBuffer;
    std::vector<vert_P1C1> DbgLineVertices;
    render_cmd_list *DbgLineRender;

    render_cmd_list *SpriteRender;
    render_resource SpriteVertexBuffer;
    std::vector<vert_P1C1UV1> SpriteVertices;

    render_cmd_list *TextRender;
    render_resource TextVertexBuffer;
    std::vector<vert_P1C1UV1> TextVertices;

    render_resource AtlasTexture;
};
#endif