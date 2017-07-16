#ifndef NEON_RENDERER_H
#define NEON_RENDERER_H

#include <ft2build.h>
#include FT_FREETYPE_H

#define NEON_INIT_GL
#define NEON_DEBUG_GL
#include "neon_GL.h"

#include "neon_math.h"
#include "neon_platform.h"
#include "neon_texture.h"
#include "neon_opengl.h"
#include <cstring> // memcpy

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
//
//	NOTE:
//	- Follow left-handed coordinate system.
//	
//	- Clock-wise vertex winding = triangle's front.
//	 
//
//		+Y
//		|
//		|		 +Z (Inward)	
//		|	   / 
//		|	  /	  
//		|	 /
//		|   /
//		|  /		(3D)
//		| /
//		|/
//	    O--------------------- +X
//
//		 +Y
//		|
//		|
//		|
//		|
//		|
//		|		(2D) 
//		|
//		| 
//		O--------------------- +X
//
//
//

// Quad vertex data
struct texture_quad
{
	GLfloat Content[54];
};
struct color_quad
{
	GLfloat Content[42];
};

texture_quad TextureQuad(vec2 Origin, vec2 Size, vec4 UVCoords, vec4 Color);
void TextureQuad(texture_quad *QuadVertex, vec2 Origin, vec2 Size, vec4 UVCoords, vec4 Color);
color_quad ColorQuad(vec2 Origin, vec2 Size, vec4 Color);

// Line data
struct line_3d
{
	// 2 vertex 
	// 1 Vertex data = Pos(3 floats) + Color (4 floats) = 7 Floats
	GLfloat Content[14];
};

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
////
////	Text rendering functions
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
	void Init();
	inline u32 UploadTexture(texture *Texture);
}
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
////
////	Render Command
////
enum render_cmd_type : u32
{
	RenderCmd_render_cmd_Clear, 
	RenderCmd_render_cmd_TextureQuad,
	RenderCmd_render_cmd_Text, 
	RenderCmd_render_cmd_ColorQuad 
};

struct render_cmd_header
{
	render_cmd_type Type;
	u64 Key;
};

struct render_cmd
{
	render_cmd_header Header;
};

struct render_cmd_Clear
{
	render_cmd_header Header;
};

struct render_cmd_TextureQuad
{
	render_cmd_header Header;
	u32  TextureIndex;
	vec2 P;
	vec2 Size;
	vec4 UV;
	vec4 Tint;
};

struct render_cmd_ColorQuad
{
	render_cmd_header Header;
	vec2 P;
	vec2 Size;
	vec4 Color;
};

struct render_cmd_Text
{
	render_cmd_header Header;
	font *Font;
	vec2 P;
	vec4 Color;
	char Text[8192];
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

void AllocRenderCmdList(render_cmd_list *RenderCmdList);
void PushRenderCmd(render_cmd_list *RenderCmdList, void *RenderCmd);
void SortRenderCmdList(render_cmd_list *RenderCmdList);
void DrawRenderCmdList(render_cmd_list *RenderCmdList);

void RenderCmdClear(render_cmd_list *RenderCmdList);
void RenderCmdTextureQuad(render_cmd_list *RenderCmdList, u32 TextureIndex, vec2 aP, vec2 aSize, vec4 aUV, vec4 aTint);
void RenderCmdColorQuad(render_cmd_list *RenderCmdList, vec2 aP, vec2 aSize, vec4 aColor);
void RenderCmdText(render_cmd_list *RenderCmdList, font *aFont, vec2 aP, vec4 aColor, char const *Fmt, ...);
#endif