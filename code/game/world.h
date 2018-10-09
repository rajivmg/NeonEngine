#ifndef WORLD_H
#define WORLD_H

#include <core/neon_platform.h>
#include <core/neon_math.h>

// NOTE: Chunk size 6x6
struct world_chunk
{
    s32 ChunkX;
    s32 ChunkY;

    u16 Tiles[16][9];
};

struct world
{
    // TODO: Hash based storage; world_chunk *Chunks[4096]; world_chunk { world_chunk *NextChunk; }
    //world_chunk Chunk;
    u16 Tiles[16][9];
    world();
    void Simulate(game_input *Input);
};
#endif