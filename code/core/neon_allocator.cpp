#include "neon_allocator.h"

void stack_allocator::Init(u64 _BufferSize)
{
    Buffer = MALLOC(_BufferSize);
    BufferSize = _BufferSize;
    Marker = (marker)Buffer;
}

void stack_allocator::Shutdown()
{
    SAFE_FREE(Buffer);
    BufferSize = 0;
    Marker = (marker)nullptr;
}

void *stack_allocator::Allocate(u64 _Size)
{
    // NOTE: If size is 0, return null
    if(_Size == 0)
    {
        return nullptr;
    }
    // NOTE: Unaligned version
    u64 TotalReqSize = _Size;
    ASSERT((Marker - (marker)Buffer + TotalReqSize) <= BufferSize);
    void *Mem = (void *)Marker;
    Marker += TotalReqSize;
    return Mem;   
}

void *stack_allocator::Allocate(u64 _Size, u32 _Alignment)
{
    // NOTE: If size is 0, return null
    if(_Size == 0)
    {
        return nullptr;
    }
    // NOTE: Aligned version
    ASSERT(_Alignment != 0);
    u64 TotalReqSize = _Size + _Alignment;
    ASSERT((Marker - (marker)Buffer + TotalReqSize) <= BufferSize);
    void *RawMem = (void *)Marker;
    Marker += TotalReqSize;

    u32 Mask = (_Alignment - 1);
    u32 Misalignment = ((uptr)RawMem & Mask);
    u32 Adjustment = _Alignment - Misalignment;
    void *AlignedMem = (void *)((uptr)RawMem + Adjustment);

    return AlignedMem;
}

stack_allocator::marker stack_allocator::GetMarker()
{
    return Marker;
}

void stack_allocator::FreeToMarker(marker _Marker)
{
    ASSERT((_Marker <= ((marker)Buffer + BufferSize)) && (_Marker >= (marker)Buffer));
    Marker = _Marker;
}

void stack_allocator::Clear()
{
    Marker = (marker)Buffer;
}
