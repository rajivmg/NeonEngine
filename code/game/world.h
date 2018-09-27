#ifndef WORLD_H
#define WORLD_H

#include <core/neon_platform.h>
#include <core/neon_math.h>
#include "cute_c2.h"
// NOTE: Chunk size 6x6
struct world_chunk
{
    s32 ChunkX;
    s32 ChunkY;

    u16 Tiles[16][9];
};

struct player
{
    vec2 P;
    //vec2 Dir;
    vec2 Speed;
    r32 MaxFootSpeed;
    bool Jumping;
    r32 JumpEffectiveDeltaY;
    bool OnGround;
};

struct world
{
    // TODO: Hash based storage; world_chunk *Chunks[4096]; world_chunk { world_chunk *NextChunk; }
    world_chunk Chunk;
    player Player;
    //vec2 GuyP;
    //bool GuyJumping;

    //vec2 GuySpeed;
    r32 Gravity;

    //r32 GuyFootSpeed;
    //r32 JumpVelocity;
    //r32 EffectiveDeltaY;
};

void GenChunk(world *World);
void WorldSimulate(game_input *Input, world *World);
#endif