#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <core/neon_platform.h>
#include <core/neon_renderer.h>
#include <core/neon_allocator.h>

#include "assetsystem.h"
#include "entitysystem.h"
#include "mapdata.h"
#include "map.h"
#include "player.h"

#include <vector>

struct game_state
{    
    r64 Time;
    r64 DeltaTime;

    game_input *Input;

    r32 MetersToPixels;
    r32 PixelsToMeters;

    mat4 ScreenProjMatrix;
    mat4 ScreenViewMatrix;
    mat4 ProjMatrix;
    mat4 ViewMatrix;

    vec2 CameraP;

    render_resource WhiteTexture;
    render_resource SpriteShader;
    render_resource TextShader;
    
    render_cmd_list *SpriteRender;

    //render_resource EntityVertexBuffer;
    //std::vector<vert_P1C1UV1> EntityVertices;

    render_cmd_list *TextRender;
    render_resource TextVertexBuffer;
    std::vector<vert_P1C1UV1> TextVertices;

    stack_allocator StackAllocator;
    entity_manager EntityManager;

    map Map;
    game_tileset CharacterTileset;

    player_entity *PlayerEntity;
};
extern game_state *GameState;
#endif