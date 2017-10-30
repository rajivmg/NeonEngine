#include "neon_renderer.h"

#include "neon_font.h"
#include "neon_opengl.h"
#include "neon_primitive_mesh.h"

//render_target RT0 = { 0, ivec2(0,0) };
const dispatch_fn cmd::udraw::DISPATCH_FUNCTION = &rndr::UnindexedDraw;

//-----------------------------------------------------------------------------
// Renderer Wrapper
//-----------------------------------------------------------------------------

void rndr::Init()
{
	// Initialise opengl function pointers.
	InitGL();
	
	// Initialise opengl renderer.
	ogl::InitState();	
}

void rndr::Clear()
{
	ogl::Clear();
}

render_resource rndr::MakeTexture(texture * Texture)
{
	return ogl::MakeTexture(Texture);
}

render_resource rndr::MakeVertexBuffer(u32 Size, bool Dynamic)
{
	return ogl::MakeVertexBuffer(Size, Dynamic);
}

void rndr::VertexBufferData(render_resource VertexBuffer, u32 Offset, u32 Size, void const *Data)
{
	ogl::VertexBufferData(VertexBuffer, Offset, Size, Data);
}

void rndr::DeleteVertexBuffer(render_resource VertexBuffer)
{
	ogl::DeleteVertexBuffer(VertexBuffer);
}

render_resource rndr::MakeIndexBuffer(u32 Size, bool Dynamic)
{
	return ogl::MakeIndexBuffer(Size, Dynamic);
}

void rndr::IndexBufferData(render_resource IndexBuffer, u32 Offset, u32 Size, void const *Data)
{
	ogl::IndexBufferData(IndexBuffer, Offset, Size, Data);
}

void rndr::DeleteIndexBuffer(render_resource IndexBuffer)
{
	ogl::DeleteIndexBuffer(IndexBuffer);
}

render_resource rndr::MakeShaderProgram(char const *VertShaderSrc, char const *FragShaderSrc)
{
	return ogl::MakeShaderProgram(VertShaderSrc, FragShaderSrc);
}

void rndr::DeleteShaderProgram(render_resource ShaderProgram)
{
	ogl::DeleteShaderProgram(ShaderProgram);
}

//render_target rndr::MakeRenderTarget(ivec2 Size, texture_filter Filter)
//{
//	render_target Result;
//	//Result = GLCreateRenderTarget(Size, Filter);
//	return Result;
//}

void rndr::UnindexedDraw(void const *Data)
{
	cmd::udraw *Cmd = (cmd::udraw *)Data;
	ogl::UnindexedDraw(Cmd);
	//Platform->Log(INFO, "TexID = %d\n", Cmd->TexID);

}

//-----------------------------------------------------------------------------
// Commands
//-----------------------------------------------------------------------------

render_cmd_list::render_cmd_list(u32 _BufferSize) : BufferSize(_BufferSize),
													BaseOffset(0),
													Current(0)
{
	const u32 MaxPacketsInList = 2048;

	Buffer = malloc(BufferSize);
	Keys = (u32 *)malloc(sizeof(u32) * MaxPacketsInList);
	Packets = (cmd_packet **)malloc(sizeof(cmd_packet *) * (MaxPacketsInList));
}

render_cmd_list::~render_cmd_list()
{
	SafeFree(Buffer)
}

void* render_cmd_list::AllocateMemory(u32 MemorySize)
{
	assert((BufferSize - BaseOffset) >= MemorySize);
	void *Mem = ((u8 *)Buffer + BaseOffset);
	BaseOffset += MemorySize;
	return Mem;
}

void render_cmd_list::Sort()
{
	// Sort using insertion sort.
	// TODO: Use Radix sort

	u32 i, j;
	for(i = 1; i < Current; ++i)
	{
		j = i;
		while((j > 0) &&  Keys[j - 1] > Keys[j])
		{
			cmd_packet *Temp = Packets[j];
			Packets[j] = Packets[j-1];
			Packets[j - 1] = Temp;
			--j;
		}
	}
}

void render_cmd_list::Submit()
{
	for(u32 I = 0; I < Current; ++I)
	{
		cmd_packet *Packet = Packets[I];
		
		for(;;)
		{
			dispatch_fn DispatchFn = Packet->DispatchFn;
			void *Cmd = Packet->Cmd;
			DispatchFn(Cmd);
			
			Packet = Packet->NextCmdPacket;
			
			if(Packet == nullptr)
				break;
		}
	}
}

void render_cmd_list::Flush()
{
	Current = 0;
	BaseOffset = 0;
}


/*
render_cmd_list* AllocRenderCmdList()
{
	render_cmd_list *Result = (render_cmd_list *)malloc(sizeof(render_cmd_list));
	
	Result->Size = MEGABYTE(8);
	Result->List = malloc(Result->Size);
	Result->Table = (void **)malloc(sizeof(void *) * 1024);
	Result->BaseOffset = 0;
	Result->CmdCount = 0;
	Result->Scratch= malloc(MEGABYTE(8));
	
	return Result;
} 

void PushRenderCmd(render_cmd_list *RenderCmdList, void *RenderCmd)
{
	Assert(RenderCmdList->List);
	
	Assert(RenderCmdList->CmdCount <= 1024);
	
	render_cmd *Cmd = (render_cmd *)RenderCmd;
	
	switch(Cmd->Header.Type)
	{
		case RenderCmd_Clear:
		{
			Assert((RenderCmdList->Size - RenderCmdList->BaseOffset) >= sizeof(render_cmd_Clear));
			
			render_cmd_Clear *Clear_Cmd = (render_cmd_Clear *)RenderCmd;
			
			render_cmd_Clear *CmdSlot;
			CmdSlot = (render_cmd_Clear *)((u8 *)RenderCmdList->List + RenderCmdList->BaseOffset);
			
			*CmdSlot = *Clear_Cmd;
			
			RenderCmdList->Table[RenderCmdList->CmdCount++] = (void *)(CmdSlot);
			RenderCmdList->BaseOffset += sizeof(render_cmd_Clear);
		} break;
		
		case RenderCmd_Line:
		{
			Assert((RenderCmdList->Size - RenderCmdList->BaseOffset) >= sizeof(render_cmd_Line));
			render_cmd_Line *Line_Cmd = (render_cmd_Line *)RenderCmd;
			
			render_cmd_Line *CmdSlot;
			CmdSlot = (render_cmd_Line *)((u8 *)RenderCmdList->List + RenderCmdList->BaseOffset);
			
			*CmdSlot = *Line_Cmd;
			
			RenderCmdList->Table[RenderCmdList->CmdCount++] = (void *)(CmdSlot);
			RenderCmdList->BaseOffset += sizeof(render_cmd_Line);
		} break;
		
		case RenderCmd_TextureQuad:
		{
			Assert((RenderCmdList->Size - RenderCmdList->BaseOffset) >= sizeof(render_cmd_TextureQuad));
			
			render_cmd_TextureQuad *TextureQuad_Cmd = (render_cmd_TextureQuad *)RenderCmd;
			
			render_cmd_TextureQuad *CmdSlot;
			CmdSlot = (render_cmd_TextureQuad *)((u8 *)RenderCmdList->List + RenderCmdList->BaseOffset);
			
			*CmdSlot = *TextureQuad_Cmd;
			
			RenderCmdList->Table[RenderCmdList->CmdCount++] = (void *)(CmdSlot);
			RenderCmdList->BaseOffset += sizeof(render_cmd_TextureQuad);
		} break;
		
		case RenderCmd_ColorQuad:
		{
			Assert((RenderCmdList->Size - RenderCmdList->BaseOffset) >= sizeof(render_cmd_ColorQuad));
			
			render_cmd_ColorQuad *ColorQuad_Cmd = (render_cmd_ColorQuad *)RenderCmd;
			
			render_cmd_ColorQuad *CmdSlot;
			CmdSlot = (render_cmd_ColorQuad *)((u8 *)RenderCmdList->List + RenderCmdList->BaseOffset);
			
			*CmdSlot = *ColorQuad_Cmd;
			
			RenderCmdList->Table[RenderCmdList->CmdCount++] = (void *)(CmdSlot);
			RenderCmdList->BaseOffset += sizeof(render_cmd_ColorQuad);
		} break;
		
		case RenderCmd_Text:
		{
			Assert((RenderCmdList->Size - RenderCmdList->BaseOffset) >= sizeof(render_cmd_Text));
			
			render_cmd_Text *Text_Cmd = (render_cmd_Text *)RenderCmd;
			
			render_cmd_Text *CmdSlot;
			CmdSlot = (render_cmd_Text *)((u8 *)RenderCmdList->List + RenderCmdList->BaseOffset);
			
			*CmdSlot = *Text_Cmd;
			
			RenderCmdList->Table[RenderCmdList->CmdCount++] = (void *)(CmdSlot);
			RenderCmdList->BaseOffset += sizeof(render_cmd_Text);
		} break;
		
		case RenderCmd_RenderTarget:
		{		
			Assert((RenderCmdList->Size - RenderCmdList->BaseOffset) >= sizeof(render_cmd_RenderTarget));

			render_cmd_RenderTarget *RenderTarget_Cmd = (render_cmd_RenderTarget *)RenderCmd;
			 
			render_cmd_RenderTarget *CmdSlot;
			CmdSlot = (render_cmd_RenderTarget *)((u8 *)RenderCmdList->List + RenderCmdList->BaseOffset);

			*CmdSlot = *RenderTarget_Cmd;

			RenderCmdList->Table[RenderCmdList->CmdCount++] = (void *)(CmdSlot);
			RenderCmdList->BaseOffset += sizeof(render_cmd_RenderTarget);
		} break;

		InvalidDefaultCase;
	}
}

void SortRenderCmdList(render_cmd_list *RenderCmdList)
{
	// Sort using insertion sort. CHANGE TO MORE EFFICIENT ALGORITHM!
	u32 i, j;
	for(i = 1; i < RenderCmdList->CmdCount; ++i)
	{
		j = i;
		while((j > 0) && 
			  ((render_cmd *)(RenderCmdList->Table[j - 1]))->Header.Key >
			  ((render_cmd *)(RenderCmdList->Table[j]))->Header.Key)
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
				case RenderCmd_Clear:
				{
					//GLClear();
				} break;
				
				case RenderCmd_Line:
				{
					render_cmd_Line *Line_Cmd = (render_cmd_Line *)RenderCmd;
					Line((line *)RenderCmdList->Scratch, Line_Cmd->Start, Line_Cmd->End, Line_Cmd->Color);
					
					u32 SameTypeCount = 0;
					while(TableIndex + 1 < RenderCmdList->CmdCount)
					{
						render_cmd *NextRenderCmd = (render_cmd *)RenderCmdList->Table[TableIndex + 1];
						if(NextRenderCmd->Header.Type == RenderCmd_Line && NextRenderCmd->Header.Target == Line_Cmd->Header.Target)
						{
							++SameTypeCount;
							render_cmd_Line *NextLine_Cmd = (render_cmd_Line *)NextRenderCmd;
							Line((line *)RenderCmdList->Scratch + SameTypeCount, NextLine_Cmd->Start, NextLine_Cmd->End, NextLine_Cmd->Color);
							++TableIndex;
						}
						else
						{
							// Next cmd is not Line cmd
							break;
						}
					}
					//GLDrawLines(RenderCmdList->Scratch, 1 + SameTypeCount, Line_Cmd->Header.Target);
				} break;
				
				case RenderCmd_TextureQuad:
				{
					render_cmd_TextureQuad *TextureQuad_Cmd = (render_cmd_TextureQuad *)RenderCmd;
					TextureQuad((texture_quad *)RenderCmdList->Scratch, TextureQuad_Cmd->P, TextureQuad_Cmd->Size, TextureQuad_Cmd->UV, TextureQuad_Cmd->Tint);
					
					u32 SameTypeCount = 0;
					while(TableIndex + 1 < RenderCmdList->CmdCount)
					{
						render_cmd *NextRenderCmd = (render_cmd *)RenderCmdList->Table[TableIndex + 1];
						if(NextRenderCmd->Header.Type == RenderCmd_TextureQuad && NextRenderCmd->Header.Target == TextureQuad_Cmd->Header.Target)
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
								// Next TextureQuad cmd has diffent texture
								break;
							}
						}
						else
						{
							// Next cmd is either not TextureQuad or has different render target
							break;
						}
					}
					
					//GLDrawTextureQuads(RenderCmdList->Scratch, 1 + SameTypeCount,  TextureQuad_Cmd->TextureIndex, TextureQuad_Cmd->Header.Target);
					
				} break;
				
				case RenderCmd_ColorQuad:
				{
					render_cmd_ColorQuad *ColorQuad_Cmd = (render_cmd_ColorQuad *)RenderCmd;
					ColorQuad((color_quad *)RenderCmdList->Scratch, ColorQuad_Cmd->P, ColorQuad_Cmd->Size, ColorQuad_Cmd->Color);
					
					u32 SameTypeCount = 0;
					while(TableIndex + 1 < RenderCmdList->CmdCount)
					{
						render_cmd *NextRenderCmd = (render_cmd *)RenderCmdList->Table[TableIndex + 1];
						if(NextRenderCmd->Header.Type == RenderCmd_ColorQuad && NextRenderCmd->Header.Target == ColorQuad_Cmd->Header.Target)
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
							// next cmd is either not ColorQuad or has different render target.
							break;
						}
					}
					//GLDrawColorQuads(RenderCmdList->Scratch, 1 + SameTypeCount, ColorQuad_Cmd->Header.Target);
					
				} break;
				
				case RenderCmd_Text:
				{
					render_cmd_Text *Text_Cmd = (render_cmd_Text *)RenderCmd;

					vec3 Pen = Text_Cmd->P;
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
						vec4 TexCoords = vec4(CharGlyph->Coords.LowerLeft.x, CharGlyph->Coords.LowerLeft.y,
											CharGlyph->Coords.UpperRight.x, CharGlyph->Coords.UpperRight.y);
						vec3 CharOrigin = vec3(Pen.x + CharGlyph->HoriBearingX, Pen.y + CharGlyph->Hang, Pen.z);
						
						TextureQuad((texture_quad *)RenderCmdList->Scratch + CharCount, 
									CharOrigin,
									vec2(CharGlyph->Width, CharGlyph->Height),
									TexCoords,
									Text_Cmd->Color);
						
						++CharCount;
						
						Pen.x += CharGlyph->HoriAdvance; 
						
						++Index;
					}
					
					//GLDrawText(RenderCmdList->Scratch, CharCount, Text_Cmd->Font->Atlas.Texture.ID, Text_Cmd->Header.Target);
					
				} break;
				
				case RenderCmd_RenderTarget:
				{
					render_cmd_RenderTarget *RenderTarget_Cmd = (render_cmd_RenderTarget *)RenderCmd;
					TextureQuad((texture_quad *)RenderCmdList->Scratch, RenderTarget_Cmd->P, RenderTarget_Cmd->Size,
						vec4(0.0f, 0.0f, 1.0f, 1.0f), vec4(1.0f, 1.0f, 1.0f, 1.0f));
					
					//GLDrawRenderTarget(RenderCmdList->Scratch, RenderTarget_Cmd->SrcRenderTarget, RenderTarget_Cmd->DestRenderTarget);
				} break;

				InvalidDefaultCase;
			}
			
			++TableIndex;
		}
		
		RenderCmdList->CmdCount = 0;
		RenderCmdList->BaseOffset = 0;
	}
	
	 //GLDrawDebugAxis();
}

void RenderCmdClear(render_cmd_list *RenderCmdList)
{
	render_cmd_Clear Cmd;
	Cmd.Header.Target = 0;
	Cmd.Header.Type = RenderCmd_Clear;
	Cmd.Header.Texture = 0;
	PushRenderCmd(RenderCmdList, &Cmd);
}

void RenderCmdLine(render_cmd_list *RenderCmdList, vec3 aStart, vec3 aEnd, vec4 aColor, render_target RenderTarget)
{
	render_cmd_Line Cmd;
	Cmd.Header.Target = (u8)RenderTarget.Handle;
	Cmd.Header.Type = RenderCmd_Line;
	Cmd.Header.Texture = 0;
	Cmd.Start = aStart;
	Cmd.End = aEnd;
	Cmd.Color = aColor;
	PushRenderCmd(RenderCmdList, &Cmd);
}

void RenderCmdTextureQuad(render_cmd_list *RenderCmdList, vec3 aP, vec2 aSize, vec4 aUV, vec4 aTint, u32 aTextureIndex, render_target RenderTarget)
{
	render_cmd_TextureQuad Cmd;
	Cmd.Header.Target = (u8)RenderTarget.Handle;
	Cmd.Header.Type = RenderCmd_TextureQuad;
	Cmd.Header.Texture = (u16)aTextureIndex;
	Cmd.TextureIndex = aTextureIndex;
	Cmd.P = aP;
	Cmd.Size = aSize;
	Cmd.UV = aUV;
	Cmd.Tint = aTint;
	PushRenderCmd(RenderCmdList, &Cmd);
}

void RenderCmdColorQuad(render_cmd_list *RenderCmdList, vec3 aP, vec2 aSize, vec4 aColor, render_target RenderTarget)
{
	render_cmd_ColorQuad Cmd;
	Cmd.Header.Target = (u8)RenderTarget.Handle;
	Cmd.Header.Type = RenderCmd_ColorQuad;
	Cmd.Header.Texture = 0;
	Cmd.P = aP;
	Cmd.Size = aSize;
	Cmd.Color = aColor;
	PushRenderCmd(RenderCmdList, &Cmd);
}

void RenderCmdText(render_cmd_list *RenderCmdList, vec3 aP, vec4 aColor, font *aFont, render_target RenderTarget, char const *Fmt, ...)
{
	render_cmd_Text Cmd;
	Cmd.Header.Target = (u8)RenderTarget.Handle;
	Cmd.Header.Type = RenderCmd_Text;
	Cmd.Font = aFont;
	Cmd.P = aP;
	Cmd.Color = aColor;
	
	Assert(aFont->Initialised);
	
	Cmd.Header.Texture = 0;
	
	va_list Arguments;
	va_start(Arguments, Fmt);
	vsnprintf(Cmd.Text, 8192, Fmt, Arguments);
	va_end(Arguments);
	
	PushRenderCmd(RenderCmdList, &Cmd);
}

void RenderCmdRenderTarget(render_cmd_list *RenderCmdList, vec3 aP, vec2 aSize, render_target SrcRenderTarget, render_target DestRenderTarget)
{
	render_cmd_RenderTarget Cmd;
	Cmd.Header.Target = (u8)255; // (u8)DestRenderTarget.Handle;
	Cmd.Header.Type = RenderCmd_RenderTarget;
	Cmd.Header.Texture = (u16)SrcRenderTarget.Handle;
	Cmd.P = aP;
	Cmd.Size = aSize;
	Cmd.SrcRenderTarget = SrcRenderTarget;
	Cmd.DestRenderTarget = DestRenderTarget;
	PushRenderCmd(RenderCmdList, &Cmd);
}

*/