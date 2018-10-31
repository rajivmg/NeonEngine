#ifndef ITEM_H
#define ITEM_H

#include "tileset.h"
#include "entitysystem.h"

struct item
{
    // NOTE: type of any item cannot be zero. zero is reserved for none item,
    // so first tile in the top left corner of a item tileset must be left blank.
    enum type { NONE, FENCE = 234 };
    type Type;
    base_entity *Entity;
};

#endif