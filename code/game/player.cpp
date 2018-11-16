#include "player.h"

#include "gamestate.h"
#include "debugdraw.h"

player_entity::player_entity()
{
    map_object *PlayerSpawnObject = GameState->Map.GetNextObjectByName("PlayerSpawn", nullptr);
    if(PlayerSpawnObject != nullptr && PlayerSpawnObject->Type == map_object::POINT)
    {
        //P = vec2(PlayerSpawnObject->x * GameState->PixelsToMeters, PlayerSpawnObject->y * GameState->PixelsToMeters);
        P = vec2(PlayerSpawnObject->x, PlayerSpawnObject->y);
    }
    else
    {
        P = vec2(15.0f, 8.0f);
    }
    Speed = 3.0f;
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

    u32 CollisionGridMinX = (u32)Clamp(0.0f, floorf(P.x), (r32)GameState->Map.Width);
    u32 CollisionGridMinY = (u32)Clamp(0.0f, floorf(P.y), (r32)GameState->Map.Height);
    u32 CollisionGridMaxX = (u32)Clamp(0.0f, floorf(P.x + 1.0f), (r32)GameState->Map.Width);
    u32 CollisionGridMaxY = (u32)Clamp(0.0f, floorf(P.y + 1.0f), (r32)GameState->Map.Height);
   
    Collider.min = c2V(P.x, P.y);
    Collider.max = c2V(P.x + 1.0f, P.y + 0.4f);

    bool Collided = false;
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
                    Collided = true;

                    P.x -= Manifold.n.x * Manifold.depths[0];
                    P.y -= Manifold.n.y * Manifold.depths[0];
                }

                if(Collided == true)
                {
                    break;
                }
            }
        }

        if(Collided == true)
        {
            break;
        }
    }

    vec2 RangeMin, RangeMax;
    RangeMin = vec2(floorf(P.x - 1.0f), floorf(P.y - 1.0f));
    RangeMax = vec2(floorf(P.x + 1.0f), floorf(P.y + 1.0f));

    u32 CursorX, CursorY;
    CursorX = (u32)floorf(((r32)GameState->Input->Mouse.x * GameState->PixelsToMeters) + GameState->CameraP.x);
    CursorY = (u32)floorf(((r32)GameState->Input->Mouse.y * GameState->PixelsToMeters) + GameState->CameraP.y);

    map_farm *Farmo;
    if(GameState->Map.CanFarm(CursorX, CursorY, &Farmo))
    {
        //DebugDraw->Text(Rect(0, 600, 0, 1), vec4(1.0f), 5.0f, "Farm Tile X: %d, Y: %d", CursorX - (u32)Farmo->Area.x, CursorY - (u32)Farmo->Area.y);
        DebugDraw->Rect(Rect((r32)CursorX, (r32)CursorY, 1.0f, 1.0f), vec4(0.6f, 0.7f, 0.0f, 0.5f), 7.0f);
    }
    else
    {
        DebugDraw->Rect(Rect((r32)CursorX, (r32)CursorY, 1.0f, 1.0f), vec4(0.9f, 0.0f, 0.0f, 0.5f), 7.0f);
        item *Item = GameState->Map.FarmGet(CursorX, CursorY, Farmo);
        if(Item)
        {
            DebugDraw->Text(Rect(0, 600, 0, 1), vec4(1.0f), 5.0f, "Item Type: %d", Item->Type);
        }
    }
    //DebugDraw->Text(Rect(0.0f, 700.0f, 1.0f, 1.0f), vec4(1.0f), 7.0f, "Cursor X, Y: %d %d", CursorX, CursorY);
    
    map_farm *Farm;
    if(GameState->Input->Mouse.Left.EndedDown && GameState->Input->Mouse.Left.HalfTransitionCount == 1)
    {
        if(GameState->Map.CanFarm(CursorX, CursorY, &Farm))
        {
            GameState->Map.FarmPut(CursorX, CursorY, Farm, item::FENCE);
        }
    }

#if 0
    // NOTE: Highlight tiles that are checked for collision
    for(u32 Y = CollisionGridMinY; Y <= CollisionGridMaxY; ++Y)
    {
        for(u32 X = CollisionGridMinX; X <= CollisionGridMaxX; ++X)
        {
            rect Rectangle = Rect(X, Y, 1.0f, 1.0f);
            DebugDraw->Rect(Rectangle, vec4(0.2f, 0.6f, 0.5f, 0.5f), 7.0f);
        }
    }
#endif
    // NOTE: Test map_object loading
    //map_object *TestRectangle = GameState->Map.GetNextObjectByName("TestRectangle", nullptr);
    //if(TestRectangle != nullptr)
    //{
    //    DebugDraw->Rect(TestRectangle->Rect, vec4(1.0f, 0.0f, 0.0f, 0.4f), 7.0f);
    //}
}

void player_entity::Draw()
{
    game_tile *CharacterTile = GameState->CharacterTileset.GetTileByID(0);
    r32 Width = GameState->CharacterTileset.TileWidthMeters;
    r32 Height = GameState->CharacterTileset.TileHeightMeters;
    PushSprite(&GameState->CharacterTileset.Vertices, Rect(P.x, P.y, Width, Height), CharacterTile->UV[0], vec4(1.0f), 0.0f, vec2(0.0f), 5.0f);
}


//NOTE: Follower camera
follow_camera_entity::follow_camera_entity()
{
    GameState->CameraP = GameState->PlayerEntity->P - vec2(15.0f, 8.4375f) + vec2(0.5f);
    Offset = GameState->CameraP - GameState->PlayerEntity->P;
    Speed = 2.3f;
}

void follow_camera_entity::Update()
{
    r32 LerpFactor = Speed * (r32)GameState->DeltaTime;

    vec2 CameraP = Lerp(GameState->CameraP, LerpFactor, GameState->PlayerEntity->P + Offset);
    
    // NOTE: Pixel snapping
    CameraP.x = floorf(CameraP.x * GameState->MetersToPixels);
    CameraP.x *= GameState->PixelsToMeters;
    CameraP.y = floorf(CameraP.y * GameState->MetersToPixels);
    CameraP.y *= GameState->PixelsToMeters;

    GameState->CameraP = CameraP;
}
