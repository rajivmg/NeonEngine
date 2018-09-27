#include "player.h"

void player_entity::Update(game_input *Input)
{
    Platform.Log("Updating Player");
    if(player_entity::TYPE_ID < snake_entity::TYPE_ID)
    {
        Platform.Log("----PlayerTypeID(%d) < SnakeTypeID(%d)", player_entity::TYPE_ID, snake_entity::TYPE_ID);
    }
    else
    {
        Platform.Log("----PlayerTypeID(%d) > SnakeTypeID(%d)", player_entity::TYPE_ID, snake_entity::TYPE_ID);
    }
}

void kermit_entity::Update(game_input *Input)
{
    Platform.Log("Updating Kermit");
    if(kermit_entity::TYPE_ID > snake_entity::TYPE_ID)
    {
        Platform.Log("----KermitTypeID(%d) > SnakeTypeID(%d)", kermit_entity::TYPE_ID, snake_entity::TYPE_ID);
    }
    else
    {
        Platform.Log("----KermitTypeID(%d) < SnakeTypeID(%d)", kermit_entity::TYPE_ID, snake_entity::TYPE_ID);
    }
}