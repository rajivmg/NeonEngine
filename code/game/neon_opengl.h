#ifndef NEON_OPENGL_H
#define NEON_OPENGL_H

#include "../platform/neon_platform.h"
#include "neon_renderer.h"

struct gl_batch
{
	GLuint VAO;
	GLuint VBO;
	GLuint IBO;

	// GLuint PROG;
	u32 PROGIndex;

	// uniform locations
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

struct gl_resources
{
	gl_state State;

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
u32 GLUploadTexture(texture *Texture, Renderer::TextureTarget Target, Renderer::TextureParam Filter, Renderer::TextureParam Wrap);
u32 GLCreateRenderTarget(u32 Width, u32 Height, Renderer::TextureParam Filter);

void GLClear();
void GLDrawLines(void *Data, u32 Count, u8 RenderTargetIndex);
void GLDrawTextureQuads(void *Data, u32 Count, u32 TextureIndex, u8 RenderTargetIndex);
void GLDrawColorQuads(void *Data, u32 Count, u8 RenderTargetIndex);
void GLDrawText(void *Data, u32 CharCount, u32 TextureIndex, u8 RenderTargetIndex);
void GLDrawTargetQuad(void *Data, u32 RenderTargetIndex);

void GLDrawDebugAxis();
#endif