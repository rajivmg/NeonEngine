#ifndef F_LEVEL_H
#define F_LEVEL_H

#include <core/neon_platform.h>
/*
Tile: 33 22 11 00 (32 bits)
ID  : 00 00 xx xx (16 bits)
Dat : xx xx 00 00 (16 bits)
*/

struct level_format
{
    u32 Magic;
};

#endif