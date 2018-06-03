#ifndef NEON_FONT_H
#define NEON_FONT_H

// Freetype
#include <ft2build.h>
#include FT_FREETYPE_H

#include "../platform/neon_platform.h"
#include "neon_bitmap.h"

struct glyph
{
    s32 Width;
    s32 Height;
    s32 HoriBearingX;
    s32 HoriBearingY;
    s32 HoriAdvance;
    s32 Hang;
    texture_coords Coords;
};

struct font
{
    glyph *Glyphs;
    u32 Height;
    bitmap_pack BitmapPack;
    render_resource FontTexture;
};

void InitFont(font *Font, char const *FontSrcFile, u32 FontHeight);
void FreeFont(font *Font);
vec2 GetTextDim(font *Font, char const *Format, ...);

#endif