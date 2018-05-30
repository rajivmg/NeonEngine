#ifndef NEON_FONT_H
#define NEON_FONT_H

// Freetype
#include <ft2build.h>
#include FT_FREETYPE_H

#include "../platform/neon_platform.h"
#include "neon_texture.h"

//-----------------------------------------------------------------------------
// Font
//-----------------------------------------------------------------------------

struct glyph
{
	s32				Width;
	s32				Height;
	s32				HoriBearingX;
	s32				HoriBearingY;
	s32				HoriAdvance;
	s32				Hang;
	texture_coords	Coords;
};

class font
{
public:
	u32 const		InstanceID;
	glyph			*Glyphs;
	u32				FontHeight;
	bitmap_pack		BitmapPack;
	render_resource FontTexture; // TODO: << Rename this.
	bool			Initialised;

	void			Load(char const * FontSrc, u32 aFontHeight);
	vec2			GetTextDim(char const *Fmt, ...);

	font();
	~font();
};

#endif