#include "player.h"

#include "gamestate.h"
#include "debugdraw.h"

player_entity::player_entity()
{
    P = vec2(15.0f, 8.0f);
    Speed = 2.0f;
}

void player_entity::Update()
{
    game_controller_input *Controller = &GameState->Input->Controllers[0];

    vec2 Direction(0.0f);

    if(Controller->Left.EndedDown)
    {
        Direction += vec2(-1.0f, 0.0f);
    }
    if(Controller->Right.EndedDown)
    {
        Direction += vec2(1.0f, 0.0f);
    }
    if(Controller->Up.EndedDown)
    {
        Direction += vec2(0.0f, 1.0f);
    }
    if(Controller->Down.EndedDown)
    {
        Direction += vec2(0.0f, -1.0f);
    }

    vec2 Velocity = Normalize(Direction) * Speed;
    P += Velocity * (r32)GameState->DeltaTime;

    u32 CollisionGridMinX = (u32)Clamp(0.0f, floorf(P.x), GameState->Map.Width);
    u32 CollisionGridMinY = (u32)Clamp(0.0f, floorf(P.y), GameState->Map.Height);
    u32 CollisionGridMaxX = (u32)Clamp(0.0f, floorf(P.x + 1.0f), GameState->Map.Width);
    u32 CollisionGridMaxY = (u32)Clamp(0.0f, floorf(P.y + 1.0f), GameState->Map.Height);
   
    Collider.min = c2V(P.x, P.y);
    Collider.max = c2V(P.x + 1.0f, P.y + 0.3f);

    for(u32 Y = CollisionGridMinY; Y <= CollisionGridMaxY; ++Y)
    {
        for(u32 X = CollisionGridMinX; X <= CollisionGridMaxX; ++X)
        {
            if(GameState->Map.CollisionLayer.Colliders[X + Y * GameState->Map.Width] == 1)
            {
                c2AABB TileCollider = { c2V((r32)X, (r32)Y), c2V((r32)X + 1, (r32)Y + 1) };
                c2Manifold Manifold;
                c2AABBtoAABBManifold(Collider, TileCollider, &Manifold);

                if(Manifold.count != 0)
                {
                    DebugDraw->Text(Rect(0, 500, 1, 1), vec4(1.0f), 5.0f, "Collision occured");
                    P.x -= Manifold.n.x * Manifold.depths[0];
                    P.y -= Manifold.n.y * Manifold.depths[0];

                    vec2 CollisionNormal = vec2(Manifold.n.x, Manifold.n.y);
                    Velocity -= CollisionNormal * Dot(Velocity, CollisionNormal);

                    P += Velocity * (r32)GameState->DeltaTime;
                }
            }
        }
    }

    //for(u32 Y = CollisionGridMinY; Y <= CollisionGridMaxY; ++Y)
    //{
    //    for(u32 X = CollisionGridMinX; X <= CollisionGridMaxX; ++X)
    //    {
    //        rect Rectangle = Rect(X * GameState->MetersToPixels + GameState->CameraP.x,
    //                              Y * GameState->MetersToPixels + GameState->CameraP.y,
    //                              1.0f * GameState->MetersToPixels, 1.0f * GameState->MetersToPixels);
    //        DebugDraw->Rect(Rectangle, vec4(0.2f, 0.6f, 0.5f, 0.5f), 7.0f);
    //    }
    //}



    /*
    Collider.min = c2V(P.x, P.y);
    Collider.max = c2V(P.x + 0.3f, P.y + 0.3f);

    // Testing grid
    u32 TestGridMinX = (u32)Clamp(0.0f, floorf(P.x), GameState->Map.Width);
    u32 TestGridMinY = (u32)Clamp(0.0f, floorf(P.y), GameState->Map.Height);
    u32 TestGridMaxX = (u32)Clamp(0.0f, floorf(P.x + 1.0f), GameState->Map.Width);
    u32 TestGridMaxY = (u32)Clamp(0.0f, floorf(P.y + 1.0f), GameState->Map.Height);

    // Debug draw testing grid
    for(u32 Y = TestGridMinY; Y <= TestGridMaxY; ++Y)
    {
        for(u32 X = TestGridMinX; X <= TestGridMaxX; ++X)
        {
            DebugDraw->Rect(Rect(X*GameState->MetersToPixels, Y*GameState->MetersToPixels, 1*GameState->MetersToPixels, 1*GameState->MetersToPixels), RGBAUnpackTo01(0xef474766), 3.0f);
        }
    }
    */
}

void player_entity::Draw()
{
    game_tile *CharacterTile = GameState->CharacterTileset.GetTileByID(0);
    PushSprite(&GameState->CharacterTileset.Vertices, Rect(P.x, P.y, 1.0f, 1.0f), CharacterTile->UV[0], vec4(1.0f), 0.0f, vec2(0.0f), 6.0f);
}


//NOTE: Follower camera
follow_camera_entity::follow_camera_entity()
{
    GameState->CameraP = GameState->PlayerEntity->P - vec2(15.0f, 8.4375f) + vec2(0.5f);
    Offset = GameState->CameraP - GameState->PlayerEntity->P;
    Speed = 2.0f;
}

void follow_camera_entity::Update()
{
    r32 LerpFactor = Speed * (r32)GameState->DeltaTime;

    vec2 CameraP = Lerp(GameState->CameraP, LerpFactor, GameState->PlayerEntity->P + Offset);
    
    // NOTE: Pixel snapping camera
    CameraP.x = floorf(CameraP.x * GameState->MetersToPixels);
    CameraP.x *= GameState->PixelsToMeters;
    CameraP.y = floorf(CameraP.y * GameState->MetersToPixels);
    CameraP.y *= GameState->PixelsToMeters;

    GameState->CameraP = CameraP;
}
