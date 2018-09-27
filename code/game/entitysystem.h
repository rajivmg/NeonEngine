#ifndef ENTITYSYSTEM_H
#define ENTITYSYSTEM_H

#include <core/neon_platform.h>
#include <core/neon_math.h>
#include <vector>

#define MAX_ENTITY_COUNT 1024

struct base_entity
{
    u32 ID;
    bool DELETED;
    
    static u32 NextTypeID();

    virtual ~base_entity() {}
    virtual void Update(game_input *Input) {}
    virtual void Draw() {}
};

template<typename T>
struct entity : base_entity
{
    static const u32 TYPE_ID;
    void operator delete(void *) = delete;
    void operator delete[](void *) = delete;
};

template<typename T>
const u32 entity<T>::TYPE_ID(base_entity::NextTypeID());

struct entity_manager
{
    base_entity *Entities[MAX_ENTITY_COUNT];
    u32 EntityCurrent;

    void Init();
    void Shutdown();

    template<typename T>
    T *AddEntity(void *Memory);

    void DeleteEntity(u32 ID);
    void Clear();

    void Update(game_input *Input);
    void Draw();
};

template<typename T>
inline T *entity_manager::AddEntity(void *Memory)
{
    T *Entity = new(Memory) T();
    Entity->ID = EntityCurrent++;
    Entity->DELETED = false;
    Entities[Entity->ID] = Entity;
    return (T *)Entities[Entity->ID];
}
#endif