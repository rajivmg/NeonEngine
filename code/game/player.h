#ifndef PLAYER_H
#define PLAYER_H

#include "entitysystem.h"

struct player_entity : entity<player_entity>
{
    bool Jumping;
    char *Name;

    void Update(game_input *Input);
    void Draw() { Platform.Log("Drawing %s", Name); }
};

struct snake_entity : entity<snake_entity>
{
    bool Poisonous;

    snake_entity() { Platform.Log("snake_entity() called"); }
    ~snake_entity() { Platform.Log("~snake_entity() called"); }
    void Update(game_input *Input) { Platform.Log("Snake is Poisonous: %d", Poisonous); }
    void Draw() { Platform.Log("Drawing Snake Entity"); }
};

struct kermit_entity : entity<kermit_entity>
{
    char *Says;
    u32 Times;

    void Update(game_input *Input);
    void Draw() { Platform.Log("Drawing Kermit Entity"); }
};
#endif