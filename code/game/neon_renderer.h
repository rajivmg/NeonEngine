#ifndef NEON_RENDERER_H
#define NEON_RENDERER_H

#include <ft2build.h>
#include FT_FREETYPE_H

#include "../platform/neon_platform.h"
#include "neon_math.h"
#include "neon_texture.h"
#include <cstring> // memcpy

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
//
//	NOTE:
//	- Follow right-handed coordinate system.
//	
//	- Counter-clockwise vertex winding = triangle's front.
//	 
//
//	 			 	(+Y)			
//	 			 	|   / (-Z)(Inward)
//	 			 	|  /		
//	 			 	| /
//	 			 	|/
//	 	(-X) --------O--------- (+X)
//	 			   /|	
//	 			  / |
//	 			 /  |					(3D)
//	 	 (+Z)	/   |
//	  	(Outward)	(-Y)
//
//
//		+Y
//		|
//		|
//		|
//		|
//		|								(2D) 
//		|
//		| 
// 		O---------------- +X
//
//
//

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
////
////	Text rendering
////
struct glyph
{
	s32 	Width;
	s32 	Height;

	s32  	HoriBearingX;
	s32  	HoriBearingY;
	s32  	HoriAdvance;

	s32		Hang;

	texture_coordinates Coordinates;
};

class font
{
public:
	glyph 	*Glyphs;
	u16		FontHeight;
	texture_atlas Atlas;
	u32 	TextureIndex;
	bool 	Initialised;

	void Load(char const * FontSrc, u16 aFontHeight);
	void FreeMemory();
	vec2 GetTextDim(char const *Fmt, ...);
	font();
	~font();
};

/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
////
////	Renderer functions
//// 
namespace Renderer
{	
	enum TextureTarget
	{
		TEXTURE_2D
	};
	enum TextureParamName
	{
		MAG_FILTER,
		MIN_FILTER,
		WRAP_S,
		WRAP_T
	};
	enum TextureParam
	{
		LINEAR,
		NEAREST,
		CLAMP_TO_EDGE,
		REPEAT
	};

	void Init();
	u32 UploadTexture(texture *Texture, Renderer::TextureTarget Target, Renderer::TextureParam Filter,
							 Renderer::TextureParam Wrap);
	u32 CreateRenderTarget(u32 Width, u32 Height, Renderer::TextureParam Filter);
}

/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
////
////	Render Command
////
enum render_cmd_type : u8
{
	RenderCmd_Clear,
	RenderCmd_ColorQuad,
	RenderCmd_TextureQuad,
	RenderCmd_Line,
	RenderCmd_Text,
	RenderCmd_RenderTargetQuad
};

struct render_cmd_header
{
	union
	{
		u32 Key;
		struct
		{
			u16 Texture;
			render_cmd_type Type;	
			u8 Target;
		};
	};
};

struct render_cmd
{
	render_cmd_header Header;
};

struct render_cmd_Clear
{
	render_cmd_header Header;
};

struct render_cmd_Line
{
	render_cmd_header Header;
	vec3 Start;
	vec3 End;
	vec4 Color;
};

struct render_cmd_TextureQuad
{
	render_cmd_header Header;
	u32  TextureIndex;
	vec3 P;
	vec2 Size;
	vec4 UV;
	vec4 Tint;
};

struct render_cmd_ColorQuad
{
	render_cmd_header Header;
	vec3 P;
	vec2 Size;
	vec4 Color;
};

struct render_cmd_Text
{
	render_cmd_header Header;
	font *Font;
	vec3 P;
	vec4 Color;
	char Text[8192];
};

struct render_cmd_RenderTargetQuad
{
	render_cmd_header Header;
	vec3 P;
	vec2 Size;
	u32  RenderTargetIndex;
};

struct render_cmd_list
{
	void *List;
	u32 BaseOffset;
	u32 Size;
	
	void **Table;
	u32 CmdCount;

	void *Scratch;
};

render_cmd_list* AllocRenderCmdList();
void PushRenderCmd(render_cmd_list *RenderCmdList, void *RenderCmd);
void SortRenderCmdList(render_cmd_list *RenderCmdList);
void DrawRenderCmdList(render_cmd_list *RenderCmdList);

void RenderCmdClear(render_cmd_list *RenderCmdList);
void RenderCmdLine(render_cmd_list *RenderCmdList, vec3 aStart, vec3 aEnd, vec4 aColor, u32 aRenderTarget);
void RenderCmdTextureQuad(render_cmd_list *RenderCmdList, vec3 aP, vec2 aSize, vec4 aUV, vec4 aTint, u32 aTextureIndex, u32 aRenderTarget);
void RenderCmdColorQuad(render_cmd_list *RenderCmdList, vec3 aP, vec2 aSize, vec4 aColor, u32 aRenderTarget);
void RenderCmdText(render_cmd_list *RenderCmdList, vec3 aP, vec4 aColor, font *aFont, u32 aRenderTarget, char const *Fmt, ...);
void RenderCmdRenderTargetQuad(render_cmd_list *RenderCmdList, vec3 aP, vec2 aSize, u32 aRenderTargetIndex);
#endif