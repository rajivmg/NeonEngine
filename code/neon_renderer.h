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

texture_quad TextureQuad(vec2 Origin, vec2 Size, vec4 Color = vec4(1.0f, 1.0f, 1.0f, 1.0f), vec4 UVCoords = vec4(0.0f, 0.0f, 1.0f, 1.0f));
color_quad ColorQuad(vec2 Origin, vec2 Size, vec4 Color);

// Line data
struct line_3d
{
	// 2 vertex 
	// 1 Vertex data = Pos(3 floats) + Color (4 floats) = 7 Floats
	GLfloat Content[14];
};

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
////
////	memory buffer
////
struct mem_buffer
{
	u32 MemAvailable;
	u32	MemUsed;
	u32 MemSize;

	void* Head;
	void* Content;
};

/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
////
////	Shader
////
class shader
{
public:
	GLuint Program;
	bool ProgramAvailable;

	void CreateProgram(read_file_result *VsFile, read_file_result *FsFile);

	shader();
	~shader();

private:
	GLuint Vs;
	GLuint Fs;
};

/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
////
////	Texture manager
////
enum tex_command // Texture manager commands
{
	GET_TEXTURE_UNIT,
	RELEASE_TEXTURE_UNIT
};

u16 TextureManager(tex_command Command, u16 TexUnit = -1);

/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
////
////	Quad Batch
////
enum quad_type
{
	TEXTURE_QUAD,
	COLOR_QUAD
};

class quad_batch
{
public:
	GLuint 	VBO;
	GLuint 	VAO;
	GLuint 	TEX;
	
	shader 	*Shader;	
	u16		TextureUnit;

	u32 	QuadCount;
	
	mem_buffer Buffer;
	
	quad_type QuadType;
	
	bool Initialised;
	bool GPUBufferAvailable;
	bool ShaderAvailable;
	bool TextureSet;
	bool FlushAfterDraw;

	void Init(quad_type aQuadType, u32 BufferSize, bool AutoFlush  = true);
	void SetTexture(texture *Texture);
	void SetShader(shader *aShader);
	void PushQuad(texture_quad *Quad);
	void PushQuad(color_quad *Quad);
	void Draw();
	void Flush();

	quad_batch();
	~quad_batch();

private:
	void CreateGPUBuffer();
	void UpdateGPUBuffer();
};

/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
////
////	Line Batch
////
class line_batch
{
public:
	GLuint 	VBO;
	GLuint 	VAO;
	
	shader 	*Shader;

	u32 	LineCount;
	
	mem_buffer Buffer;

	bool Initialised;
	bool GPUBufferAvailable;
	bool ShaderAvailable;
	bool FlushAfterDraw;

	void Init(u32 BufferSize, bool AutoFlush  = true);
	void SetShader(shader *aShader);
	void PushLine(vec3 Start, vec3 End, vec4 Color);
	void PushLine(vec3 Start, r32 Radian, u32 Length, vec4 Color);
	void Draw();
	void Flush();

	line_batch();
	~line_batch();

private:
	void CreateGPUBuffer();
	void UpdateGPUBuffer();
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
	bool 	Initialised;

	void Load(char const * FontSrc, u16 aFontHeight);
	void FreeMemory();
	font();
	~font();
};

class text_batch
{
public:
	quad_batch Batch;
	font *Font;
	bool FontSet;

	vec2 Pen;

	void SetFont(font *aFont, u32 BufferSize);
	void PushText(vec2 Origin, vec4 Color, char const *Fmt, ...);
	void Draw();
	void Flush();

	text_batch();
	~text_batch();
};


/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
////
////	Renderer functions
//// 
struct render_resources
{
	shader 	*TQuadShader;
	shader 	*CQuadShader;
	shader  *TextShader;
	font 	*HUDFont;
} RenderResources;

void InitRenderer();
void ClearBackBuffer();

#endif