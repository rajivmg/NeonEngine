#ifndef NEON_ALLOCATOR_H
#define NEON_ALLOCATOR_H

#include "neon_platform.h"

struct stack_allocator
{
    typedef uptr marker;
    
    void *Buffer;
    u64 BufferSize;
    marker Marker;

    void Init(u64 _BufferSize);
    void Shutdown();

    void *Allocate(u64 _Size);
    void *Allocate(u64 _Size, u32 _Alignment);
    marker GetMarker();
    void FreeToMarker(marker _Marker);
    void Clear();
};

#endif