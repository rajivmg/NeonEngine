#include "entitysystem.h"

#include "gamestate.h"

static u32 EntityTypeIDCounter = 0;

u32 base_entity::NextTypeID()
{
    return EntityTypeIDCounter++;
}

void entity_manager::Init()
{
    memset(Entities, 0, sizeof(base_entity *) * MAX_ENTITY_COUNT);
    EntityCurrent = 0;
}

void entity_manager::Shutdown()
{
    Clear();
}

void entity_manager::DeleteEntity(u32 ID)
{
    ASSERT(!Entities[ID]->DELETED);

    Entities[ID]->DELETED = true;
    Entities[ID]->~base_entity();
}

void entity_manager::Clear()
{
    for(u32 I = 0; I < EntityCurrent; ++I)
    {
        if(!Entities[I]->DELETED)
        {
            DeleteEntity(Entities[I]->ID); // NOTE: Simple I should work too!
        }
    }

    memset(Entities, 0, sizeof(base_entity *) * MAX_ENTITY_COUNT);
    EntityCurrent = 0;
}

void entity_manager::Update()
{
    for(u32 I = 0; I < EntityCurrent; ++I)
    {
        if(!Entities[I]->DELETED)
        {
            Entities[I]->Update();
        }
    }
}

void entity_manager::Draw()
{
    for(u32 I = 0; I < EntityCurrent; ++I)
    {
        if(!Entities[I]->DELETED)
        {
            Entities[I]->Draw();
        }
    }
}
