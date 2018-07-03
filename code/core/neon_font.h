#ifndef NEON_FONT_H
#define NEON_FONT_H

// Freetype
#include <ft2build.h>
#include FT_FREETYPE_H

#include "neon_platform.h"
#include "neon_bitmap.h"

struct glyph
{
    u32 Id;
    rect Rect;
    s32 XOffset;
    s32 YOffset;
    s32 XAdvance;
};

struct font
{
    font(const char *FontFile);
    ~font();
    
    void Free();
    glyph *GetGlyph(u32 Codepoint);

    u32 LineHeight;
    u32 Base;
    u32 ScaleW;
    u32 ScaleH;
    render_resource Texture;
    
    u32 GlyphsCount;
    glyph *Glyphs;
};

#if 0 
struct glyph
{
    s32 Width;
    s32 Height;
    s32 HoriBearingX;
    s32 HoriBearingY;
    s32 HoriAdvance;
    s32 Hang;
    vec4 Coords;
};

struct font
{
    u32 Height;
    glyph *Glyphs;
    render_resource Texture;
};

void InitFont(font *Font, char const *FontSrcFile, u32 FontHeight);
void FreeFont(font *Font);
vec2 GetTextDim(font *Font, char const *Format, ...);
#endif
#endif