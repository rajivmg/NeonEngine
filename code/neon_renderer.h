#ifndef NEON_RENDERER_H
#define NEON_RENDERER_H

#include <ft2build.h>
#include FT_FREETYPE_H

#include "neon_math.h"
#include "neon_platform.h"
#include "neon_texture.h"
#include "neon_primitive_mesh.h"
#include "neon_mesh.h"
#include "neon_opengl.h"
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
	void Init();
	inline u32 UploadTexture(texture *Texture);
	inline u32 UploadMesh(mesh *Mesh);
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
	RenderCmd_render_cmd_ColorQuad,
	RenderCmd_render_cmd_Mesh
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

struct render_cmd_Mesh
{
	render_cmd_header Header;
	u32 MeshIndex;
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
void RenderCmdTextureQuad(render_cmd_list *RenderCmdList, vec3 aP, vec2 aSize, vec4 aUV, u32 TextureIndex, vec4 aTint);
void RenderCmdColorQuad(render_cmd_list *RenderCmdList, vec3 aP, vec2 aSize, vec4 aColor);
void RenderCmdText(render_cmd_list *RenderCmdList, font *aFont, vec3 aP, vec4 aColor, char const *Fmt, ...);
void RenderCmdMesh(render_cmd_list *RenderCmdList, u32 aMeshIndex);
#endif