#include "world.h"

#include "debugdraw.h"

void GenChunk(world *World)
{
    u16 Map[9][16] =
    {
        {12, 12, 12, 12 ,12, 12, 12, 0, 0, 12, 12, 12, 12, 12, 12, 12},
        {12, 12, 12, 12 ,12, 12, 12, 0, 0, 12, 12, 12, 12, 12, 12, 12},
        {12, 12, 12, 12 ,12, 12, 12, 0, 0, 12, 12, 12, 12, 12, 12, 12},
        {12, 12, 12, 12 ,12, 12, 12, 0, 0, 12, 0, 0, 12, 12, 12, 12},
        {12, 12, 12, 12 ,12, 12, 12, 0, 0, 0, 0, 12, 12, 12, 12, 12},
        {12, 12, 12, 12 ,12, 12, 12, 0, 0, 0, 12, 12, 12, 12, 12, 12},
        {12, 12, 12, 12 ,12, 12, 12, 0, 0, 12, 12, 12, 12, 12, 12, 12},
        {12, 12, 12, 12 ,12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12},
        {12, 12, 12, 12 ,12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12}
    };

    // TODO: Generate or load world
    for(u32 Y = 0; Y < ARRAY_COUNT(world_chunk::Tiles[0]); ++Y)
    {
        for(u32 X = 0; X < ARRAY_COUNT(world_chunk::Tiles); ++X)
        {
            World->Chunk.Tiles[X][Y] = Map[8 - Y][X];
        }
    }
}

void JumpBegin(player *Player)
{
    const r32 PeekHeight = 1.2f;
    const r32 PeekHeightHoriDist = 0.5f;

    Player->Jumping = true;
    Player->Speed.x = 0.0f;
    Player->Speed.y = (2.0f * PeekHeight * Player->MaxFootSpeed) / PeekHeightHoriDist;
    Player->JumpEffectiveDeltaY = 0.0f;
    //Player->OnGround = false;
}

void JumpEnd(player *Player)
{

}

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
            if(World->Chunk.Tiles[X][Y] == 12)
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

void PlayerUpdate(world *World, game_input *Input)
{
    game_controller_input *Controller = &Input->Controllers[0];
    player *Player = &World->Player;

    vec2 MoveDir(0.0f);
    if(Controller->Left.EndedDown)
    {
        MoveDir += vec2(-1.0f, 0.0f);
    }
    if(Controller->Right.EndedDown)
    {
        MoveDir += vec2(1.0f, 0.0f);
    }
    //if(Controller->Up.EndedDown)
    //{
    //    MoveDir += vec2(0.0f, 1.0f);
    //}
    //if(Controller->Down.EndedDown)
    //{
    //    MoveDir += vec2(0.0f, -1.0f);
    //}
    if(!Player->Jumping && Controller->Jump.EndedDown && Controller->Jump.HalfTransitionCount == 1)
    {
        JumpBegin(&World->Player);
    }

    r32 DeltaT = (r32)Input->DeltaTime;
    r32 DeltaTSqr = (r32)(Input->DeltaTime * Input->DeltaTime);

    // Gravity
    if(!Player->OnGround && !Player->Jumping)
    {
        r32 DeltaY = Player->Speed.y * DeltaT + (0.5f * World->Gravity * DeltaTSqr);
        Player->P.y += DeltaY;
        CheckEntityCollision(World);
        Player->Speed.y += World->Gravity * DeltaT;
        Player->Speed.y = Clamp(-11.0f, Player->Speed.y, 11.0f);
    }
    else
    {
        CheckEntityCollision(World);
    }

    Player->P += Player->MaxFootSpeed * MoveDir * DeltaT;

    if(Player->Jumping)
    {
        r32 DeltaY = Player->Speed.y * DeltaT + (0.5f * World->Gravity * DeltaTSqr);
        Player->P.y += DeltaY;
        CheckEntityCollision(World);
        Player->Speed.y += World->Gravity * DeltaT;
        Player->Speed.y = Clamp(-11.0f, Player->Speed.y, 11.0f);

        Player->JumpEffectiveDeltaY += DeltaY;
        if(Player->Jumping && Player->JumpEffectiveDeltaY <= 0.0f)
        {
            Player->Jumping = false;
            //Player->Speed.y = 0.0f;
        }
    }

    if(Player->OnGround)
    {
        Player->JumpEffectiveDeltaY = 0.0f;
        Player->Speed.y = 0.0f;
    }

    DebugDraw->Rect(Rect(50, 500, 200, 180), RGBAUnpackTo01(0xffffff99), 2.9f);
    DebugDraw->Text(Rect(50, 660, 10, 10), RGBAUnpackTo01(0x0000FFFF), 3.0f, "OnGround : %d", Player->OnGround);
    DebugDraw->Text(Rect(50, 640, 10, 10), RGBAUnpackTo01(0x0000FFFF), 3.0f, "Jumping  : %d", Player->Jumping);
    DebugDraw->Text(Rect(50, 620, 10, 10), RGBAUnpackTo01(0x0000FFFF), 3.0f, "SpeedX   : %0.3f", Player->Speed.x);
    DebugDraw->Text(Rect(50, 600, 10, 10), RGBAUnpackTo01(0x0000FFFF), 3.0f, "SpeedY   : %0.3f", Player->Speed.y);
    DebugDraw->Text(Rect(50, 580, 10, 10), RGBAUnpackTo01(0x0000FFFF), 3.0f, "JumpDeltaY: %0.3f", Player->JumpEffectiveDeltaY);
}

void WorldSimulate(game_input *Input, world *World)
{
    PlayerUpdate(World, Input);

    //r32 DeltaT = (r32)Input->DeltaTime;
    //r32 DeltaTSqr = (r32)(Input->DeltaTime * Input->DeltaTime);
    //r32 Y = World->JumpVelocity * DeltaT + (0.5f * World->Gravity * DeltaTSqr);
    //Platform.Log("Y: %f", Y);
    //World->GuyP.y += Y;

    //if(World->GuyJumping)
    //{
    //    World->JumpVelocity += World->Gravity * DeltaT;
    //    World->EffectiveDeltaY += Y;
    //    if(World->EffectiveDeltaY <= 0.0f)
    //    {
    //        World->GuyJumping = false;
    //    }
    //}

    //r32 GuySpeed = 1.5f; // NOTE: 1.5 meter/sec
    //World->GuyP += GuySpeed * GuyDirection * Input->DeltaTime;
    
    //c2AABB GuyAABB;
    //GuyAABB.min = c2V(World->GuyP.x, World->GuyP.y);
    //GuyAABB.max = c2V(World->GuyP.x + 1.0f, World->GuyP.y + 1.0f);

    //u32 TestGridMinX = (u32)floorf(World->GuyP.x);
    //u32 TestGridMinY = (u32)floorf(World->GuyP.y);
    //u32 TestGridMaxX = (u32)ceilf(World->GuyP.x);
    //u32 TestGridMaxY = (u32)ceilf(World->GuyP.y);
    //
    //for(u32 Y = TestGridMinY; Y <= TestGridMaxY; ++Y)
    //{
    //    for(u32 X = TestGridMinX; X <= TestGridMaxX; ++X)
    //    {
    //        DebugDraw->Rect(Rect(X*80, Y*80, 1*80, 1*80), RGBAUnpackTo01(0xef474766), 3.0f);
    //    }
    //}

    //bool BreakTest = false;
    //bool GuyColliding = false;
    ////for(s32 I = 0; I < 2; ++I)
    ////{
    //    for(u32 Y = TestGridMinY; Y <= TestGridMaxY; ++Y)
    //    {
    //        for(u32 X = TestGridMinX; X <= TestGridMaxX; ++X)
    //        {
    //            if(World->Chunk.Tiles[X][Y] == 12)
    //            {
    //                // NOTE: Do collision test against this tile
    //                c2Manifold M;
    //                c2AABB TileAABB;// = { c2V(X, Y), c2V(X + 1, Y + 1) };
    //                TileAABB.min = c2V(X, Y);
    //                TileAABB.max = c2V(X + 1, Y + 1);
    //                c2AABBtoAABBManifold(GuyAABB, TileAABB, &M);
    //                if(M.count != 0)
    //                {
    //                    GuyColliding = true;
    //                    //World->GuyJumping = false;
    //                    World->GuyP.x -= M.n.x * M.depths[0];
    //                    World->GuyP.y -= M.n.y * M.depths[0];
    //                    BreakTest = true;
    //                    break;
    //                }
    //            }
    //        }

    //        if(BreakTest)
    //        {
    //            break;
    //        }
    //    }
    //}

    //DebugDraw->Rect(Rect(50, 580, 200, 80), RGBAUnpackTo01(0xffffff99), 2.9f);
    //DebugDraw->Text(Rect(50, 660, 10, 10), RGBAUnpackTo01(0x0000FFFF), 3.0f, "Guy Colliding: %d", GuyColliding);
    //DebugDraw->Text(Rect(50, 640, 10, 10), RGBAUnpackTo01(0x0000FFFF), 3.0f, "Guy Jumping  : %d", World->GuyJumping);
}