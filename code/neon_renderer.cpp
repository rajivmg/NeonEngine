#include "neon_renderer.h"

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////
////	Quad
////
void TextureQuad(texture_quad *Quad, vec2 Origin, vec2 Size, vec4 UVCoords, vec4 Color)
{
	/*
	D--------C
	|  U 	/|
	|      / |
	|  	  /	 |
	|    /   |
	|	/    |
	|  /     |
	| /      |
	|/    B  |
	A--------B
	
	A.XYZ = Origin.XYZ
	A.UV  = UVCoords.xY

	B.X   = Origin.X + Size.x - 1
	B.Y   = Origin.Y
	B.Z   = 0
	B.U   = UVCoords.z 
	B.V   = UVCoords.y 

	C.X   = Origin.X + Size.x - 1
	C.Y   = Origin.Y + Size.y -1
	C.Z   = 0
	C.UV  = UVCoords.zW

	D.X   = Origin.X 
	D.Y   = Origin.Y + Size.y - 1
	D.Z   = 0
	D.U   = UVCoords.x
	D.V   = UVCoords.w
 */
	Quad->Content[0] = Origin.x;
	Quad->Content[1] = Origin.y + Size.y;
	Quad->Content[2] = 0;

	Quad->Content[3]  = Color.r;
	Quad->Content[4]  = Color.g;
	Quad->Content[5]  = Color.b;
	Quad->Content[6]  = Color.a;

	Quad->Content[7]  = UVCoords.x;
	Quad->Content[8]  = UVCoords.w;

	// C
	Quad->Content[9]  = Origin.x + Size.x;
	Quad->Content[10] = Origin.y + Size.y;
	Quad->Content[11] = 0;

	Quad->Content[12] = Color.r;
	Quad->Content[13] = Color.g;
	Quad->Content[14] = Color.b;
	Quad->Content[15] = Color.a;

	Quad->Content[16] = UVCoords.z;
	Quad->Content[17] = UVCoords.w;

	// A
	Quad->Content[18] = Origin.x;
	Quad->Content[19] = Origin.y;
	Quad->Content[20] = 0;

	Quad->Content[21] = Color.r;
	Quad->Content[22] = Color.g;
	Quad->Content[23] = Color.b;
	Quad->Content[24] = Color.a;
  
	Quad->Content[25] = UVCoords.x;
	Quad->Content[26] = UVCoords.y;


	// Lower triangle
	// A
	Quad->Content[27] = Origin.x;
	Quad->Content[28] = Origin.y;
	Quad->Content[29] = 0;

	Quad->Content[30] = Color.r;
	Quad->Content[31] = Color.g;
	Quad->Content[32] = Color.b;
	Quad->Content[33] = Color.a;
  
	Quad->Content[34] = UVCoords.x;
	Quad->Content[35] = UVCoords.y;

	// C
	Quad->Content[36] = Origin.x + Size.x;
	Quad->Content[37] = Origin.y + Size.y;
	Quad->Content[38] = 0;

	Quad->Content[39] = Color.r;
	Quad->Content[40] = Color.g;
	Quad->Content[41] = Color.b;
	Quad->Content[42] = Color.a;

	Quad->Content[43] = UVCoords.z;
	Quad->Content[44] = UVCoords.w;

	// B
	Quad->Content[45] = Origin.x + Size.x;
	Quad->Content[46] = Origin.y;
	Quad->Content[47] = 0;

	Quad->Content[48] = Color.r;
	Quad->Content[49] = Color.g;
	Quad->Content[50] = Color.b;
	Quad->Content[51] = Color.a;

	Quad->Content[52] = UVCoords.z;
	Quad->Content[53] = UVCoords.y;
}

void ColorQuad(color_quad *Quad, vec2 Origin, vec2 Size, vec4 Color)
{
	// Upper triangle
	// D
	Quad->Content[0] = Origin.x;
	Quad->Content[1] = Origin.y + Size.y;
	Quad->Content[2] = 0;

	Quad->Content[3]  = Color.r;
	Quad->Content[4]  = Color.g;
	Quad->Content[5]  = Color.b;
	Quad->Content[6]  = Color.a;

	// C
	Quad->Content[7]  = Origin.x + Size.x;
	Quad->Content[8] = Origin.y + Size.y;
	Quad->Content[9] = 0;

	Quad->Content[10] = Color.r;
	Quad->Content[11] = Color.g;
	Quad->Content[12] = Color.b;
	Quad->Content[13] = Color.a;

	// A
	Quad->Content[14] = Origin.x;
	Quad->Content[15] = Origin.y;
	Quad->Content[16] = 0;

	Quad->Content[17] = Color.r;
	Quad->Content[18] = Color.g;
	Quad->Content[19] = Color.b;
	Quad->Content[20] = Color.a;

	// Lower triangle
	// A
	Quad->Content[21] = Origin.x;
	Quad->Content[22] = Origin.y;
	Quad->Content[23] = 0;

	Quad->Content[24] = Color.r;
	Quad->Content[25] = Color.g;
	Quad->Content[26] = Color.b;
	Quad->Content[27] = Color.a;

	// C
	Quad->Content[28] = Origin.x + Size.x;
	Quad->Content[29] = Origin.y + Size.y;
	Quad->Content[30] = 0;

	Quad->Content[31] = Color.r;
	Quad->Content[32] = Color.g;
	Quad->Content[33] = Color.b;
	Quad->Content[34] = Color.a;

	// B
	Quad->Content[35] = Origin.x + Size.x;
	Quad->Content[36] = Origin.y;
	Quad->Content[37] = 0;

	Quad->Content[38] = Color.r;
	Quad->Content[39] = Color.g;
	Quad->Content[40] = Color.b;
	Quad->Content[41] = Color.a;
}

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
////
////	Font
////
font::font() : Initialised(false)
{
}

font::~font()
{
}

void font::Load(char const * FontSrc, u16 aFontHeight)
{
	FontHeight = aFontHeight;
	Glyphs = (glyph *)malloc(sizeof(glyph) * (128-32));
	Atlas.Initialise(512, 512, 2); // padding = 2

	FT_Library FTLib;
	FT_Face Face;

	int Error = FT_Init_FreeType(&FTLib);
	if(Error)
	{
		Assert(!"Error initialising the FreeType library.");
	}

	read_file_result FontData = Platform->ReadFile(FontSrc);

	Error = FT_New_Memory_Face(FTLib, (const FT_Byte *)FontData.Content, FontData.ContentSize, 0, &Face);
	if(Error == FT_Err_Unknown_File_Format)
	{
		Assert(!"File format is not supported.");
	}
	else if(Error)
	{
		Assert(!"File could not be opened or read.");
	}

	Error = FT_Set_Pixel_Sizes(Face, 0, FontHeight);
	Assert(Error == 0);

	texture *GlyphTexture = (texture *)malloc(sizeof(texture));
	for(int CIndex = 32; CIndex < 128; ++CIndex)
	{
		int GlyphIndex = FT_Get_Char_Index(Face, CIndex);
	
		Error = FT_Load_Glyph(Face, GlyphIndex, FT_LOAD_DEFAULT);
		Assert(Error == 0);
		
		if(Face->glyph->format != FT_GLYPH_FORMAT_BITMAP)
		{
			FT_Render_Glyph(Face->glyph, FT_RENDER_MODE_NORMAL);
		}

		glyph *Glyph = Glyphs + (CIndex - 32);
		Glyph->Width = Face->glyph->bitmap.width;
		Glyph->Height = Face->glyph->bitmap.rows;
		
		u8 *GlyphBitmap = Face->glyph->bitmap.buffer;

		FT_Glyph_Metrics *Metrics = &Face->glyph->metrics;

		Glyph->HoriBearingX = Metrics->horiBearingX/64;
		Glyph->HoriBearingY = Metrics->horiBearingY/64;
		Glyph->HoriAdvance  = Metrics->horiAdvance/64;
		Glyph->Hang 		= Glyph->HoriBearingY - Glyph->Height;
		
		GlyphTexture->Width = Glyph->Width;
		GlyphTexture->Height = Glyph->Height;
		GlyphTexture->ContentSize = GlyphTexture->Width * GlyphTexture->Height * 4;
		if(GlyphTexture->ContentSize != 0)
		{
			GlyphTexture->Content = malloc(GlyphTexture->ContentSize);
			Assert(GlyphTexture != 0);
			memset(GlyphTexture->Content, 255, GlyphTexture->ContentSize);
		}
		
		u32 *DestTexel = (u32 *)GlyphTexture->Content;
		u8 *SrcTexel = (u8 *)GlyphBitmap;

		// convert Glpyh's 8bit texture to 32bit texture.
		for(int x = 0; x < GlyphTexture->Width * GlyphTexture->Height; ++x)
		{
			u8 *DestGreen = (u8 *)DestTexel+3;
			*DestGreen = *SrcTexel;

			DestTexel++;
			SrcTexel++;
		}

		if(GlyphTexture->ContentSize != 0)
		{
			// char Temp[20];
			// snprintf(Temp, 20, "%d.tga", CIndex);
			// DebugTextureSave(Temp, GlyphTexture);

			Glyph->Coordinates = Atlas.PackTexture(GlyphTexture);
		}

		// After glyph texture has been copied to texture atlas free
		// glyph texture memory.
		if(GlyphTexture->ContentSize != 0)
		{
			free(GlyphTexture->Content);
		}
	}
	
	Atlas.GenTexture();

	//@HACK: see texture::PackTexture()
	Atlas.Texture.FlippedVertically = true;
	TextureIndex = Atlas.Texture.UploadToGPU();
	Initialised = true;


	// debugging purpose
	// DebugTextureSave("FontAtlas.tga", &Atlas.Texture);
}

vec2 font::GetTextDim(char const *Fmt, ...)
{
	vec2 Result(0, 0);

	char Text[8192];

	va_list Arguments;
	va_start(Arguments, Fmt);
	vsnprintf(Text, 8192, Fmt, Arguments);
	va_end(Arguments);

	Assert(Initialised);

	vec2 Dim(0,	0);
	Dim.y = FontHeight;
	s32 MaxHang = 0;
	s32 PrevX = 0;
	int Index = 0;
	while(Text[Index] != 0)
	{
		if((int)Text[Index] == 10)
		{
			Dim.y += FontHeight;
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

void font::FreeMemory()
{
	Assert(Initialised);
	SAFE_FREE(Glyphs);
	Atlas.FreeMemory();
}


void Renderer::Init()
{
	// initialise opengl function pointers.
	InitGL();

	// initialise opengl renderer.
	GLInitRenderer();
}

inline
u32 Renderer::UploadTexture(texture *Texture)
{
	Assert(Texture->Initialised);

	s32 Index;
	Index = GLUploadTexture(Texture);
	Texture->OnGPU = true;
	return Index;
}

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
////
////	Render command
////
void AllocRenderCmdList(render_cmd_list *RenderCmdList)
{
	RenderCmdList->Size = MEGABYTE(8);
	RenderCmdList->List = malloc(RenderCmdList->Size);
	RenderCmdList->Table = (void **)malloc(sizeof(void *) * 1024);
	RenderCmdList->BaseOffset = 0;
	RenderCmdList->CmdCount = 0;

	RenderCmdList->Scratch= malloc(MEGABYTE(8));
}	

void PushRenderCmd(render_cmd_list *RenderCmdList, void *RenderCmd)
{
	Assert(RenderCmdList->List);

	Assert(RenderCmdList->CmdCount <= 1024);
	
	render_cmd *Cmd = (render_cmd *)RenderCmd;

	switch(Cmd->Header.Type)
	{
		case RenderCmd_render_cmd_Clear:
		{
			Assert((RenderCmdList->Size - RenderCmdList->BaseOffset) >= sizeof(render_cmd_Clear));

			render_cmd_Clear *Clear_Cmd = (render_cmd_Clear *)RenderCmd;

			render_cmd_Clear *CmdSlot;
			CmdSlot = (render_cmd_Clear *)((u8 *)RenderCmdList->List + RenderCmdList->BaseOffset);

			*CmdSlot = *Clear_Cmd;

			RenderCmdList->Table[RenderCmdList->CmdCount++] = (void *)(CmdSlot);
			RenderCmdList->BaseOffset += sizeof(render_cmd_Clear);
		} break;

		case RenderCmd_render_cmd_TextureQuad:
		{
			Assert((RenderCmdList->Size - RenderCmdList->BaseOffset) >= sizeof(render_cmd_TextureQuad));

			render_cmd_TextureQuad *TextureQuad_Cmd = (render_cmd_TextureQuad *)RenderCmd;

			render_cmd_TextureQuad *CmdSlot;
			CmdSlot = (render_cmd_TextureQuad *)((u8 *)RenderCmdList->List + RenderCmdList->BaseOffset);

			*CmdSlot = *TextureQuad_Cmd;
			
			RenderCmdList->Table[RenderCmdList->CmdCount++] = (void *)(CmdSlot);
			RenderCmdList->BaseOffset += sizeof(render_cmd_TextureQuad);
		} break;

		case RenderCmd_render_cmd_ColorQuad:
		{
			Assert((RenderCmdList->Size - RenderCmdList->BaseOffset) >= sizeof(render_cmd_ColorQuad));

			render_cmd_ColorQuad *ColorQuad_Cmd = (render_cmd_ColorQuad *)RenderCmd;

			render_cmd_ColorQuad *CmdSlot;
			CmdSlot = (render_cmd_ColorQuad *)((u8 *)RenderCmdList->List + RenderCmdList->BaseOffset);

			*CmdSlot = *ColorQuad_Cmd;
			
			RenderCmdList->Table[RenderCmdList->CmdCount++] = (void *)(CmdSlot);
			RenderCmdList->BaseOffset += sizeof(render_cmd_ColorQuad);
		} break;

		case RenderCmd_render_cmd_Text:
		{
			Assert((RenderCmdList->Size - RenderCmdList->BaseOffset) >= sizeof(render_cmd_Text));

			render_cmd_Text *Text_Cmd = (render_cmd_Text *)RenderCmd;

			render_cmd_Text *CmdSlot;
			CmdSlot = (render_cmd_Text *)((u8 *)RenderCmdList->List + RenderCmdList->BaseOffset);

			*CmdSlot = *Text_Cmd;
			
			RenderCmdList->Table[RenderCmdList->CmdCount++] = (void *)(CmdSlot);
			RenderCmdList->BaseOffset += sizeof(render_cmd_Text);
		} break;

		InvalidDefaultCase;
	}
}

void SortRenderCmdList(render_cmd_list *RenderCmdList)
{
	// Sort using insertion sort
	u8 i, j;
	for(i = 1; i < RenderCmdList->CmdCount; ++i)
	{
		j = i;
		while((j > 0) && 
				((render_cmd *)(RenderCmdList->Table[j - 1]))->Header.Type >
				((render_cmd *)(RenderCmdList->Table[j]))->Header.Type)
		{
			void *Temp = RenderCmdList->Table[j];
			RenderCmdList->Table[j] = RenderCmdList->Table[j-1];
			RenderCmdList->Table[j - 1] = Temp;
			--j;
		}
	}
}

void DrawRenderCmdList(render_cmd_list *RenderCmdList)
{
	if(!RenderCmdList->CmdCount == 0)
	{
		SortRenderCmdList(RenderCmdList);

		u32 TableIndex = 0;
		while(TableIndex < RenderCmdList->CmdCount)
		{
			render_cmd *RenderCmd = (render_cmd *)RenderCmdList->Table[TableIndex];
	
			switch(RenderCmd->Header.Type)
			{
				case RenderCmd_render_cmd_Clear:
				{
					GLClear();
				} break;

				case RenderCmd_render_cmd_TextureQuad:
				{
					render_cmd_TextureQuad *TextureQuad_Cmd = (render_cmd_TextureQuad *)RenderCmd;
					TextureQuad((texture_quad *)RenderCmdList->Scratch, TextureQuad_Cmd->P, TextureQuad_Cmd->Size, TextureQuad_Cmd->UV, TextureQuad_Cmd->Tint);
					
					u32 SameTypeCount = 0;
					while(1)
					{
						render_cmd *NextRenderCmd = (render_cmd *)RenderCmdList->Table[TableIndex + 1];
						if(NextRenderCmd->Header.Type == RenderCmd_render_cmd_TextureQuad)
						{
							render_cmd_TextureQuad *NextTextureQuad_Cmd = (render_cmd_TextureQuad *)NextRenderCmd;
							if(NextTextureQuad_Cmd->TextureIndex == TextureQuad_Cmd->TextureIndex)
							{
								++SameTypeCount;
								TextureQuad((texture_quad *)RenderCmdList->Scratch + SameTypeCount, 
											NextTextureQuad_Cmd->P, 
											NextTextureQuad_Cmd->Size, 
											NextTextureQuad_Cmd->UV, 
											NextTextureQuad_Cmd->Tint);
								++TableIndex;
							}
							else
							{
								// next TextureQuad cmd has diffent texture.
								break;
							}
						}
						else
						{
							// next cmd is not TextureQuad.
							break;
						}
					}

					// extract TextureIndex from Key
					// u64 Index = TextureQuad_Cmd->Header.Key & 0x00000000FFFFFFFF;

					GLDrawTextureQuads(RenderCmdList->Scratch, 1 + SameTypeCount,  TextureQuad_Cmd->TextureIndex);

				} break;
	
				case RenderCmd_render_cmd_ColorQuad:
				{
					render_cmd_ColorQuad *ColorQuad_Cmd = (render_cmd_ColorQuad *)RenderCmd;
					ColorQuad((color_quad *)RenderCmdList->Scratch, ColorQuad_Cmd->P, ColorQuad_Cmd->Size, ColorQuad_Cmd->Color);

					u32 SameTypeCount = 0;
					while(TableIndex + 1 < RenderCmdList->CmdCount)
					{
						render_cmd *NextRenderCmd = (render_cmd *)RenderCmdList->Table[TableIndex + 1];
						if(NextRenderCmd->Header.Type == RenderCmd_render_cmd_ColorQuad)
						{
							++SameTypeCount;
							render_cmd_ColorQuad *NextColorQuad_Cmd = (render_cmd_ColorQuad *)NextRenderCmd;
							ColorQuad((color_quad *)RenderCmdList->Scratch + SameTypeCount,
										NextColorQuad_Cmd->P,
										NextColorQuad_Cmd->Size,
										NextColorQuad_Cmd->Color);
							++TableIndex;
						}
						else
						{
							// next cmd is not ColorQuad.
							break;
						}
					}
					GLDrawColorQuads(RenderCmdList->Scratch, 1 + SameTypeCount);

				} break;
	
				case RenderCmd_render_cmd_Text:
				{
					render_cmd_Text *Text_Cmd = (render_cmd_Text *)RenderCmd;

					Assert(Text_Cmd->Font->Initialised); //@NOTE: Not required, already checked

					vec2 Pen = Text_Cmd->P;
					Pen.y -= Text_Cmd->Font->FontHeight;

					u32 CharCount = 0;
					int Index = 0;
					while(Text_Cmd->Text[Index] != 0)
					{
						if((int)Text_Cmd->Text[Index] == 10)
						{
							Pen.x = Text_Cmd->P.x;
							Pen.y -= Text_Cmd->Font->FontHeight;
							++Index;
							continue;
						}

						glyph *CharGlyph = Text_Cmd->Font->Glyphs + ((int)Text_Cmd->Text[Index] - 32);
						vec4 TexCoords = vec4(CharGlyph->Coordinates.BL_X, CharGlyph->Coordinates.BL_Y, CharGlyph->Coordinates.TR_X, CharGlyph->Coordinates.TR_Y);
						vec2 CharOrigin = vec2(Pen.x + CharGlyph->HoriBearingX, Pen.y + CharGlyph->Hang);

						TextureQuad((texture_quad *)RenderCmdList->Scratch + CharCount, 
									CharOrigin,
									vec2(CharGlyph->Width, CharGlyph->Height),
									TexCoords,
									Text_Cmd->Color);
						
						++CharCount;

						Pen.x += CharGlyph->HoriAdvance; 

						++Index;
					}
					
					GLDrawText(RenderCmdList->Scratch, CharCount, Text_Cmd->Font->TextureIndex);

				} break;
					
				InvalidDefaultCase;
			}

			++TableIndex;
		}

		RenderCmdList->CmdCount = 0;
		RenderCmdList->BaseOffset = 0;
	}

	GLDrawDebugAxis();
}

void RenderCmdClear(render_cmd_list *RenderCmdList)
{
	render_cmd_Clear Cmd;
	Cmd.Header.Type = RenderCmd_render_cmd_Clear;
	Cmd.Header.Key  = ((u64)RenderCmd_render_cmd_Clear << 32) | 0; 
	PushRenderCmd(RenderCmdList, &Cmd);
}

void RenderCmdTextureQuad(render_cmd_list *RenderCmdList, u32 TextureIndex, vec2 aP, vec2 aSize, vec4 aUV, vec4 aTint)
{
	// 00 00 00 00 00 00 00 01
	render_cmd_TextureQuad Cmd;
	Cmd.Header.Type = RenderCmd_render_cmd_TextureQuad;
	Cmd.Header.Key	= ((u64)RenderCmd_render_cmd_TextureQuad << 32) | TextureIndex;
	Cmd.TextureIndex = TextureIndex;
	Cmd.P = aP;
	Cmd.Size = aSize;
	Cmd.UV = aUV;
	Cmd.Tint = aTint;
	PushRenderCmd(RenderCmdList, &Cmd);
}

void RenderCmdColorQuad(render_cmd_list *RenderCmdList, vec2 aP, vec2 aSize, vec4 aColor)
{
	render_cmd_ColorQuad Cmd;
	Cmd.Header.Type = RenderCmd_render_cmd_ColorQuad;
	Cmd.Header.Key	= ((u64)RenderCmd_render_cmd_ColorQuad << 32) | 0;
	Cmd.P = aP;
	Cmd.Size = aSize;
	Cmd.Color = aColor;
	PushRenderCmd(RenderCmdList, &Cmd);
}

void RenderCmdText(render_cmd_list *RenderCmdList, font *aFont, vec2 aP, vec4 aColor, char const *Fmt, ...)
{
	render_cmd_Text Cmd;
	Cmd.Header.Type = RenderCmd_render_cmd_Text;
	Cmd.Font = aFont;
	Cmd.P = aP;
	Cmd.Color = aColor;

	Assert(aFont->Initialised);

	Cmd.Header.Key = ((u64)RenderCmd_render_cmd_Text << 32) | 0;

	va_list Arguments;
	va_start(Arguments, Fmt);
	vsnprintf(Cmd.Text, 8192, Fmt, Arguments);
	va_end(Arguments);

	PushRenderCmd(RenderCmdList, &Cmd);
}