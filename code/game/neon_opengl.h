#ifndef NEON_OPENGL_H
#define NEON_OPENGL_H

#include "neon_GL.h"

#include "../platform/neon_platform.h"
#include "neon_math.h"

class texture;
struct render_resource;

namespace cmd
{
	struct udraw;
}

enum class texture_type;
enum class texture_filter;
enum class texture_wrap;

struct buffer_object
{
	GLuint	Buffer;
	s32		Capacity;
	bool	IsDynamic;
};

struct shader_program
{
	GLuint	Program;
	u32		Sampler2DCount;
};

struct render_state
{
	GLuint			Texture[1024];
	s32				TextureCount;
	buffer_object	VertexBuffer[2048];
	s32				VertexBufferCount;
	buffer_object	IndexBuffer[2048];
	s32				IndexBufferCount;
	shader_program	ShaderProgram[1024];
	s32				ShaderProgramCount;

	mat4			OrthoProjection;
};

namespace ogl
{
	void			InitState();
	void			Clear();
	render_resource MakeTexture(texture *Texture);
	render_resource	MakeVertexBuffer(u32 Size, bool Dynamic = true);
	void			VertexBufferData(render_resource VertexBuffer, u32 Offset, u32 Size, void const *Data);
	void			DeleteVertexBuffer(render_resource VertexBuffer);
	render_resource	MakeIndexBuffer(u32 Size, bool Dynamic = true);
	void			IndexBufferData(render_resource IndexBuffer, u32 Offset, u32 Size, void const *Data);
	void			DeleteIndexBuffer(render_resource IndexBuffer);
	render_resource MakeShaderProgram(char const *VertShaderSrc, char const *FragShaderSrc);
	void			DeleteShaderProgram(render_resource ShaderProgram);
	void			UnindexedDraw(cmd::udraw *Cmd);
}
#endif

/*
struct gl_batch
{
	GLuint VAO;
	GLuint VBO;
	GLuint IBO;

	u32 PROGIndex;

	// Uniform locations
	GLuint map0Uniform;
	GLuint projectionUniform;
};

struct gl_state
{
	u8 BoundRenderTarget;
};	

struct gl_texture
{
	GLuint TEXHandle;
};

struct gl_shader_program
{
	GLuint PROGHandle;
};

struct gl_render_target
{
	GLuint FBOHandle;
	GLuint ColorTEXHandle;
	GLuint DepthRBOHandle;
};

struct gl_camera
{
	mat4 CamMatrix;
};

struct gl_resources
{
	gl_state State;

	gl_camera Camera;

	gl_texture Texture[256];
	u32 TextureIndex;

	gl_shader_program ShaderProgram[64];
	u32 ShaderProgramIndex;

	gl_render_target RenderTarget[16];
	u32 RenderTargetIndex;
};

static void GLInitTextureQuadBatch();
static void GLInitColorQuadBatch();
static void GLInitTextBatch();
static void GLInitLineBatch();

void GLInitRenderer();

u32 GLCreateProgram(read_file_result *VsFile, read_file_result *FsFile);
u32 GLUploadTexture(texture *Texture, texture_param Target, texture_param Filter, texture_param Wrap);
render_target GLCreateRenderTarget(ivec2 Size, texture_param Filter);

void GLClear();
void GLDrawLines(void *Data, u32 Count, u8 RenderTargetIndex);
void GLDrawTextureQuads(void *Data, u32 Count, u32 TextureIndex, u8 RenderTargetIndex);
void GLDrawColorQuads(void *Data, u32 Count, u8 RenderTargetIndex);
void GLDrawText(void *Data, u32 CharCount, u32 TextureIndex, u8 RenderTargetIndex);
void GLDrawRenderTarget(void *Data, render_target SrcRenderTarget, render_target DestRenderTarget);

void GLDrawDebugAxis();
*/