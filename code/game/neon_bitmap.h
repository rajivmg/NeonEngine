#ifndef NEON_TEXTURE_H
#define NEON_TEXTURE_H

#include "../platform/neon_platform.h"
#include "neon_math.h"

#include <cstdlib> // malloc

#include "neon_renderer.h"

// TGA file header format.
#pragma pack(push, 1)
typedef struct tga_header
{
    u8  IDLength;
    u8  ColorMapType;
    u8  ImageType;
    u16 CMapStart;
    u16 CMapLength;
    u8  CMapDepth;
    u16 XOffset;
    u16 YOffset;
    u16 Width;
    u16 Height;
    u8  PixelDepth;
    u8  ImageDescriptor;
} tga_header;
#pragma pack(pop)

enum class texture_type
{
    TEXTURE_2D
};

enum class texture_filter
{
    LINEAR, NEAREST
};

enum class texture_wrap
{
    CLAMP, REPEAT
};

struct bitmap
{
    u32 Width;
    u32 Height;
    void *Data;
    u32 DataSize;
    u32 BytesPerPixel;
    bool FlippedAroundY;
};

void LoadBitmap(bitmap *Bitmap, char const *Filename);
void FreeBitmap(bitmap *Bitmap);
void BitmapFlipAroundY(bitmap *Bitmap);

struct texture_coords
{
    vec2 LowerLeft;
    vec2 UpperRight;
};

struct texture_rect
{
    u32 OriginX;
    u32 OriginY;

    u32 Width;
    u32 Height;
};

struct binary_t_node
{
    binary_t_node *Child[2];

    texture_rect Rect;
    bool Filled;
};

struct bitmap_pack
{
    bitmap Bitmap;
    u32 Padding;

    binary_t_node Node; // [Internal use only]
};

void InitBitmapPack(bitmap_pack *BitmapPack, u32 Width, u32 Height, u32 Padding);
texture_coords BitmapPackInsert(bitmap_pack *BitmapPack, bitmap *Bitmap);

#ifdef DEBUG
#define DebugTextureSave(F, T) DebugTextureSave_(F, T)
#else
#define DebugTextureSave(F, T)
#endif

// Save texture object on disk in .tga format
// Note that the first byte of texture data will be saved as the first top left pixel.
void DebugTextureSave_(char const *Filename, bitmap *Bitmap);

#endif