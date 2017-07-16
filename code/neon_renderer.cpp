#include "neon_renderer.h"

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////
////	Quad
////
texture_quad TextureQuad(vec2 Origin, vec2 Size, vec4 UVCoords, vec4 Color)
{
	texture_quad QuadVertex;
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

	// Upper triangle
	// D
	QuadVertex.Content[0] = Origin.x;
	QuadVertex.Content[1] = Origin.y + Size.y;
	QuadVertex.Content[2] = 0;

	QuadVertex.Content[3]  = Color.r;
	QuadVertex.Content[4]  = Color.g;
	QuadVertex.Content[5]  = Color.b;
	QuadVertex.Content[6]  = Color.a;

	QuadVertex.Content[7]  = UVCoords.x;
	QuadVertex.Content[8]  = UVCoords.w;

	// C
	QuadVertex.Content[9]  = Origin.x + Size.x;
	QuadVertex.Content[10] = Origin.y + Size.y;
	QuadVertex.Content[11] = 0;

	QuadVertex.Content[12] = Color.r;
	QuadVertex.Content[13] = Color.g;
	QuadVertex.Content[14] = Color.b;
	QuadVertex.Content[15] = Color.a;

	QuadVertex.Content[16] = UVCoords.z;
	QuadVertex.Content[17] = UVCoords.w;

	// A
	QuadVertex.Content[18] = Origin.x;
	QuadVertex.Content[19] = Origin.y;
	QuadVertex.Content[20] = 0;

	QuadVertex.Content[21] = Color.r;
	QuadVertex.Content[22] = Color.g;
	QuadVertex.Content[23] = Color.b;
	QuadVertex.Content[24] = Color.a;
  
	QuadVertex.Content[25] = UVCoords.x;
	QuadVertex.Content[26] = UVCoords.y;


	// Lower triangle
	// A
	QuadVertex.Content[27] = Origin.x;
	QuadVertex.Content[28] = Origin.y;
	QuadVertex.Content[29] = 0;

	QuadVertex.Content[30] = Color.r;
	QuadVertex.Content[31] = Color.g;
	QuadVertex.Content[32] = Color.b;
	QuadVertex.Content[33] = Color.a;
  
	QuadVertex.Content[34] = UVCoords.x;
	QuadVertex.Content[35] = UVCoords.y;

	// C
	QuadVertex.Content[36] = Origin.x + Size.x;
	QuadVertex.Content[37] = Origin.y + Size.y;
	QuadVertex.Content[38] = 0;

	QuadVertex.Content[39] = Color.r;
	QuadVertex.Content[40] = Color.g;
	QuadVertex.Content[41] = Color.b;
	QuadVertex.Content[42] = Color.a;

	QuadVertex.Content[43] = UVCoords.z;
	QuadVertex.Content[44] = UVCoords.w;

	// B
	QuadVertex.Content[45] = Origin.x + Size.x;
	QuadVertex.Content[46] = Origin.y;
	QuadVertex.Content[47] = 0;

	QuadVertex.Content[48] = Color.r;
	QuadVertex.Content[49] = Color.g;
	QuadVertex.Content[50] = Color.b;
	QuadVertex.Content[51] = Color.a;

	QuadVertex.Content[52] = UVCoords.z;
	QuadVertex.Content[53] = UVCoords.y;

	return QuadVertex;
}

void TextureQuad(texture_quad *QuadVertex, vec2 Origin, vec2 Size, vec4 UVCoords, vec4 Color)
{
	QuadVertex->Content[0] = Origin.x;
	QuadVertex->Content[1] = Origin.y + Size.y;
	QuadVertex->Content[2] = 0;

	QuadVertex->Content[3]  = Color.r;
	QuadVertex->Content[4]  = Color.g;
	QuadVertex->Content[5]  = Color.b;
	QuadVertex->Content[6]  = Color.a;

	QuadVertex->Content[7]  = UVCoords.x;
	QuadVertex->Content[8]  = UVCoords.w;

	// C
	QuadVertex->Content[9]  = Origin.x + Size.x;
	QuadVertex->Content[10] = Origin.y + Size.y;
	QuadVertex->Content[11] = 0;

	QuadVertex->Content[12] = Color.r;
	QuadVertex->Content[13] = Color.g;
	QuadVertex->Content[14] = Color.b;
	QuadVertex->Content[15] = Color.a;

	QuadVertex->Content[16] = UVCoords.z;
	QuadVertex->Content[17] = UVCoords.w;

	// A
	QuadVertex->Content[18] = Origin.x;
	QuadVertex->Content[19] = Origin.y;
	QuadVertex->Content[20] = 0;

	QuadVertex->Content[21] = Color.r;
	QuadVertex->Content[22] = Color.g;
	QuadVertex->Content[23] = Color.b;
	QuadVertex->Content[24] = Color.a;
  
	QuadVertex->Content[25] = UVCoords.x;
	QuadVertex->Content[26] = UVCoords.y;


	// Lower triangle
	// A
	QuadVertex->Content[27] = Origin.x;
	QuadVertex->Content[28] = Origin.y;
	QuadVertex->Content[29] = 0;

	QuadVertex->Content[30] = Color.r;
	QuadVertex->Content[31] = Color.g;
	QuadVertex->Content[32] = Color.b;
	QuadVertex->Content[33] = Color.a;
  
	QuadVertex->Content[34] = UVCoords.x;
	QuadVertex->Content[35] = UVCoords.y;

	// C
	QuadVertex->Content[36] = Origin.x + Size.x;
	QuadVertex->Content[37] = Origin.y + Size.y;
	QuadVertex->Content[38] = 0;

	QuadVertex->Content[39] = Color.r;
	QuadVertex->Content[40] = Color.g;
	QuadVertex->Content[41] = Color.b;
	QuadVertex->Content[42] = Color.a;

	QuadVertex->Content[43] = UVCoords.z;
	QuadVertex->Content[44] = UVCoords.w;

	// B
	QuadVertex->Content[45] = Origin.x + Size.x;
	QuadVertex->Content[46] = Origin.y;
	QuadVertex->Content[47] = 0;

	QuadVertex->Content[48] = Color.r;
	QuadVertex->Content[49] = Color.g;
	QuadVertex->Content[50] = Color.b;
	QuadVertex->Content[51] = Color.a;

	QuadVertex->Content[52] = UVCoords.z;
	QuadVertex->Content[53] = UVCoords.y;
}

color_quad ColorQuad(vec2 Origin, vec2 Size, vec4 Color)
{

	color_quad QuadVertex;
	// Upper triangle
	// D
	QuadVertex.Content[0] = Origin.x;
	QuadVertex.Content[1] = Origin.y + Size.y;
	QuadVertex.Content[2] = 0;

	QuadVertex.Content[3]  = Color.r;
	QuadVertex.Content[4]  = Color.g;
	QuadVertex.Content[5]  = Color.b;
	QuadVertex.Content[6]  = Color.a;

	// C
	QuadVertex.Content[7]  = Origin.x + Size.x;
	QuadVertex.Content[8] = Origin.y + Size.y;
	QuadVertex.Content[9] = 0;

	QuadVertex.Content[10] = Color.r;
	QuadVertex.Content[11] = Color.g;
	QuadVertex.Content[12] = Color.b;
	QuadVertex.Content[13] = Color.a;

	// A
	QuadVertex.Content[14] = Origin.x;
	QuadVertex.Content[15] = Origin.y;
	QuadVertex.Content[16] = 0;

	QuadVertex.Content[17] = Color.r;
	QuadVertex.Content[18] = Color.g;
	QuadVertex.Content[19] = Color.b;
	QuadVertex.Content[20] = Color.a;

	// Lower t.angle
	// A
	QuadVertex.Content[21] = Origin.x;
	QuadVertex.Content[22] = Origin.y;
	QuadVertex.Content[23] = 0;

	QuadVertex.Content[24] = Color.r;
	QuadVertex.Content[25] = Color.g;
	QuadVertex.Content[26] = Color.b;
	QuadVertex.Content[27] = Color.a;

	// C
	QuadVertex.Content[28] = Origin.x + Size.x;
	QuadVertex.Content[29] = Origin.y + Size.y;
	QuadVertex.Content[30] = 0;

	QuadVertex.Content[31] = Color.r;
	QuadVertex.Content[32] = Color.g;
	QuadVertex.Content[33] = Color.b;
	QuadVertex.Content[34] = Color.a;

	// B
	QuadVertex.Content[35] = Origin.x + Size.x;
	QuadVertex.Content[36] = Origin.y;
	QuadVertex.Content[37] = 0;

	QuadVertex.Content[38] = Color.r;
	QuadVertex.Content[39] = Color.g;
	QuadVertex.Content[40] = Color.b;
	QuadVertex.Content[41] = Color.a;

	return QuadVertex;
}

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
////
////	Font
////
font::font() : Initialised(false)
{
	// Initialised = false;
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
	Atlas.Texture.FlipedVertically = true;
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
			CmdSlot = (render_cmd_Clear *)((s8 *)RenderCmdList->List + RenderCmdList->BaseOffset);

			*CmdSlot = *Clear_Cmd;

			RenderCmdList->Table[RenderCmdList->CmdCount++] = (void *)(CmdSlot);
			RenderCmdList->BaseOffset += sizeof(render_cmd_Clear);
		} break;

		case RenderCmd_render_cmd_TextureQuad:
		{
			Assert((RenderCmdList->Size - RenderCmdList->BaseOffset) >= sizeof(render_cmd_TextureQuad));

			render_cmd_TextureQuad *TextureQuad_Cmd = (render_cmd_TextureQuad *)RenderCmd;

			render_cmd_TextureQuad *CmdSlot;
			CmdSlot = (render_cmd_TextureQuad *)((s8 *)RenderCmdList->List + RenderCmdList->BaseOffset);

			*CmdSlot = *TextureQuad_Cmd;
			
			RenderCmdList->Table[RenderCmdList->CmdCount++] = (void *)(CmdSlot);
			RenderCmdList->BaseOffset += sizeof(render_cmd_TextureQuad);
		} break;

		case RenderCmd_render_cmd_ColorQuad:
		{
			Assert((RenderCmdList->Size - RenderCmdList->BaseOffset) >= sizeof(render_cmd_ColorQuad));

			render_cmd_ColorQuad *ColorQuad_Cmd = (render_cmd_ColorQuad *)RenderCmd;

			render_cmd_ColorQuad *CmdSlot;
			CmdSlot = (render_cmd_ColorQuad *)((s8 *)RenderCmdList->List + RenderCmdList->BaseOffset);

			*CmdSlot = *ColorQuad_Cmd;
			
			RenderCmdList->Table[RenderCmdList->CmdCount++] = (void *)(CmdSlot);
			RenderCmdList->BaseOffset += sizeof(render_cmd_ColorQuad);
		} break;

		case RenderCmd_render_cmd_Text:
		{
			Assert((RenderCmdList->Size - RenderCmdList->BaseOffset) >= sizeof(render_cmd_Text));

			render_cmd_Text *Text_Cmd = (render_cmd_Text *)RenderCmd;

			render_cmd_Text *CmdSlot;
			CmdSlot = (render_cmd_Text *)((s8 *)RenderCmdList->List + RenderCmdList->BaseOffset);

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
					color_quad Quad = ColorQuad(ColorQuad_Cmd->P, ColorQuad_Cmd->Size, ColorQuad_Cmd->Color);
					GLDrawColorQuads(&Quad, 1);

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