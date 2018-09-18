#ifndef GAME_H
#define GAME_H

#include <core/neon_platform.h>
#include <core/neon_bitmap.h>
#include <core/neon_font.h>

#include <vector>

#include "asset.h"
#include "world.h"

struct game_state
{
    r32 MetersToPixels;
    r32 PixelsToMeters;

    mat4 ScreenProjMatrix;
    mat4 ScreenViewMatrix;
    mat4 ProjMatrix;
    mat4 ViewMatrix;

    render_resource WhiteTexture;
    render_resource SpriteShader;
    render_resource TextShader;

    render_cmd_list *SpriteRender;
    render_resource SpriteVertexBuffer;
    std::vector<vert_P1C1UV1> SpriteVertices;

    render_cmd_list *TextRender;
    render_resource TextVertexBuffer;
    std::vector<vert_P1C1UV1> TextVertices;

    render_resource AtlasTexture;
};
#endif