#include "neon_font.h"

#include "neon_renderer.h"

//-----------------------------------------------------------------------------
// Font
//-----------------------------------------------------------------------------

font::font() : InstanceID(GEN_ID),
			   Glyphs(0),
			   Initialised(false)
{
}

font::~font()
{
	SAFE_FREE(Glyphs);
}

void font::Load(char const *FontSrc, u32 aFontHeight)
{
	FontHeight = aFontHeight;
	Glyphs = (glyph *)malloc(sizeof(glyph) * (128 - 32));
	
	InitBitmapPack(&BitmapPack, 512, 512, 2);

	FT_Library FTLib;
	FT_Face Face;

	int Error = FT_Init_FreeType(&FTLib);
	if(Error)
	{
		assert(!"Error initialising the FreeType library.");
	}

	file_content FontData = Platform.ReadFile(FontSrc);
	assert(FontData.NoError);

	Error = FT_New_Memory_Face(FTLib, (const FT_Byte *)FontData.Content, (FT_Long)FontData.Size, 0, &Face);
	if(Error == FT_Err_Unknown_File_Format)
	{
		assert(!"File format is not supported.");
	}
	else if(Error)
	{
		assert(!"File could not be opened or read.");
	}

	Error = FT_Set_Pixel_Sizes(Face, 0, FontHeight);
	assert(Error == 0);

	bitmap *GlyphTexture = new bitmap();
	for(int CIndex = 32; CIndex < 128; ++CIndex)
	{
		int GlyphIndex = FT_Get_Char_Index(Face, CIndex);

		Error = FT_Load_Glyph(Face, GlyphIndex, FT_LOAD_DEFAULT);
		assert(Error == 0);

		if(Face->glyph->format != FT_GLYPH_FORMAT_BITMAP)
		{
			FT_Render_Glyph(Face->glyph, FT_RENDER_MODE_NORMAL);
		}

		glyph *Glyph = Glyphs + (CIndex - 32);
		Glyph->Width = Face->glyph->bitmap.width;
		Glyph->Height = Face->glyph->bitmap.rows;

		u8 *GlyphBitmap = Face->glyph->bitmap.buffer;

		FT_Glyph_Metrics *Metrics = &Face->glyph->metrics;

		Glyph->HoriBearingX = Metrics->horiBearingX / 64;
		Glyph->HoriBearingY = Metrics->horiBearingY / 64;
		Glyph->HoriAdvance = Metrics->horiAdvance / 64;
		Glyph->Hang = Glyph->HoriBearingY - Glyph->Height;

		GlyphTexture->Width = Glyph->Width;
		GlyphTexture->Height = Glyph->Height;
		GlyphTexture->DataSize = GlyphTexture->Width * GlyphTexture->Height * 4;
		if(GlyphTexture->DataSize != 0)
		{
			GlyphTexture->Data = malloc(GlyphTexture->DataSize);
			assert(GlyphTexture != 0);
			memset(GlyphTexture->Data, 255, GlyphTexture->DataSize);
		}

		u32 *DestTexel = (u32 *)GlyphTexture->Data;
		u8 *SrcTexel = (u8 *)GlyphBitmap;

		// Convert Glpyh's 8bit texture to 32bit texture
		for(u32 x = 0; x < GlyphTexture->Width * GlyphTexture->Height; ++x)
		{
			u8 *DestGreen = (u8 *)DestTexel + 3;
			*DestGreen = *SrcTexel;

			DestTexel++;
			SrcTexel++;
		}

		if(GlyphTexture->DataSize != 0)
		{
			Glyph->Coords = BitmapPackInsert(&BitmapPack, GlyphTexture);
		}

		// After glyph texture has been copied to texture atlas free the glyph texture memory
		if(GlyphTexture->DataSize != 0)
		{
			SAFE_FREE(GlyphTexture->Data);
		}
	}

	//TextureAtlas.Texture.CreateRenderResource();
	FontTexture = rndr::MakeTexture(&BitmapPack.Bitmap, texture_type::TEXTURE_2D, texture_filter::LINEAR, texture_wrap::CLAMP, false);

	Initialised = true;

	Platform.FreeFileContent(&FontData);
	// Debugging
	// DebugTextureSave("FontAtlas.tga", &Atlas.Texture);
}

vec2 font::GetTextDim(char const *Fmt, ...)
{
	assert(Initialised);

	vec2 Result(0, 0);

	char Text[8192];

	va_list Arguments;
	va_start(Arguments, Fmt);
	vsnprintf(Text, 8192, Fmt, Arguments);
	va_end(Arguments);

	vec2 Dim(0, 0);
	Dim.y = (r32)FontHeight;
	s32 MaxHang = 0;
	s32 PrevX = 0;
	int Index = 0;
	while(Text[Index] != 0)
	{
		if((int)Text[Index] == 10)
		{
			Dim.y += (r32)FontHeight;
			++Index;
			MaxHang = 0;
			PrevX = (s32)Dim.x;
			Dim.x = 0;
			continue;
		}

		glyph *CharGlyph = Glyphs + ((int)Text[Index] - 32);
		Dim.x += CharGlyph->HoriAdvance;
		++Index;

		if(CharGlyph->Hang * -1 > MaxHang)
		{
			MaxHang = CharGlyph->Hang * -1;
		}
	}


	Result.x = (Dim.x >= PrevX ? Dim.x : PrevX);
	Result.y = (Dim.y + MaxHang);
	return Result;
}