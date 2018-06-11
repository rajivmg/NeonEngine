#ifndef NEON_GAME_H
#define NEON_GAME_H

#include "../platform/neon_platform.h"

#include "neon_renderer.h"
#include "neon_math.h"
#include "neon_bitmap.h"
#include "neon_text.h"

platform_t Platform;


struct game_state
{
    render_resource WhiteTexture;

    render_cmd_list *WorldRender;
    render_resource WorldVertexBuffer;
    std::vector<vert_P1C1UV1> WorldVertices;

    render_resource SpriteShader;

    font DbgFont;

    render_resource TextShader;

    render_resource DbgTextVertexBuffer;
    render_cmd_list *DbgTextRender;

    render_resource DbgLineShader;
    render_resource DbgLineVertexBuffer;
    std::vector<vert_P1C1> DbgLineVertices;
    render_cmd_list *DbgLineRender;

    r32 MetersToPixels;
    r32 PixelsToMeters;

    render_resource WonderArtTexture;
};

#endif