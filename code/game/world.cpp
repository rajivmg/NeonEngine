#include "world.h"

#include "cute_c2.h"
#include "debugdraw.h"

world::world()
{
    u16 Map[9][16] =
    {
        {0, 1, 1, 1 ,1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {0, 1, 1, 1 ,1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {0, 1, 1, 1 ,1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {0, 0, 0, 0 ,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {0, 1, 1, 1 ,1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1},
        {0, 1, 1, 1 ,1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1},
        {0, 1, 1, 1 ,1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1},
        {0, 1, 1, 1 ,1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1},
        {0, 0, 0, 0 ,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1}
    };

    // TODO: Generate or load world
    for(u32 Y = 0; Y < ARRAY_COUNT(world_chunk::Tiles[0]); ++Y)
    {
        for(u32 X = 0; X < ARRAY_COUNT(world_chunk::Tiles); ++X)
        {
            Tiles[X][Y] = Map[8 - Y][X];
        }
    }
}
void world::Simulate(game_input *Input)
{

}
/*
void CheckEntityCollision(world *World)
{
    player *Player = &World->Player;
    // Testing grid
    u32 TestGridMinX = (u32)Clamp(0.0f, floorf(Player->P.x), 15.0f);
    u32 TestGridMinY = (u32)Clamp(0.0f, floorf(Player->P.y), 8.0f);
    u32 TestGridMaxX = (u32)Clamp(0.0f, floorf(Player->P.x + 1.0f), 15.0f);
    u32 TestGridMaxY = (u32)Clamp(0.0f, floorf(Player->P.y + 1.0f), 8.0f);

    // Debug draw testing grid
    for(u32 Y = TestGridMinY; Y <= TestGridMaxY; ++Y)
    {
        for(u32 X = TestGridMinX; X <= TestGridMaxX; ++X)
        {
            DebugDraw->Rect(Rect(X*80, Y*80, 1*80, 1*80), RGBAUnpackTo01(0xef474766), 3.0f);
        }
    }

    // Object aabb
    c2AABB A = { c2V(Player->P.x, Player->P.y) , c2V(Player->P.x + 1.0f, Player->P.y + 1.0f) };

    // Collide Object with tiles in testing grid
    bool CollidedWithGround = false;
    for(u32 Y = TestGridMinY; Y <= TestGridMaxY; ++Y)
    {
        for(u32 X = TestGridMinX; X <= TestGridMaxX; ++X)
        {
            if(World->Chunk.Tiles[X][Y] == 1)
            {
                // NOTE: Do collision test against this tile
                c2AABB TileAABB;
                TileAABB.min = c2V(X, Y);
                TileAABB.max = c2V(X + 1, Y + 1);
                c2Manifold M;
                c2AABBtoAABBManifold(A, TileAABB, &M);
                if(M.count != 0)
                {
                    if(M.n.y == -1.0f) { CollidedWithGround = true; }
                   
                    Player->P.x -= M.n.x * M.depths[0];
                    Player->P.y -= M.n.y * M.depths[0];
                }
            }
        }
    }
    if(!CollidedWithGround)
    {
        Player->OnGround = false;
    }
    else
    {
        Player->OnGround = true;
    }
}
*/