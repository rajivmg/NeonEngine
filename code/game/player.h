#ifndef PLAYER_H
#define PLAYER_H

#include "entitysystem.h"
#include "cute_c2.h"

struct player_entity : entity<player_entity>
{
    vec2 P;
    r32 Speed;

    c2AABB Collider;

    player_entity();
    
    void Update();
    void Draw();
};

struct follow_camera_entity : entity<follow_camera_entity>
{
    vec2 Offset;
    r32 Speed;

    follow_camera_entity();
    
    void Update();
};
#endif