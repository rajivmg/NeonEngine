#ifndef NEON_FONT_H
#define NEON_FONT_H
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
    u32 LineHeight;
    u32 Base;
    u32 ScaleW;
    u32 ScaleH;
    render_resource Texture;
    
    u32 GlyphsCount;
    glyph *Glyphs;
};

void InitFont(font *Font, const char *FontFile);
void FreeFont(font *Font);
glyph *FontGetGlyph(font *Font, u32 Codepoint);
#endif