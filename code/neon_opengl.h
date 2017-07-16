#ifndef NEON_OPENGL_H
#define NEON_OPENGL_H

#include "neon_platform.h"

struct gl_batch
{
	GLuint VAO;
	GLuint VBO;
	GLuint TEX;
	GLuint PROG;

	// Uniform locations
	GLuint map0Uniform;
	GLuint ProjMatUniform;
};

struct gl_state
{
	s32 TextureUnit[8];
};

struct gl_resources
{
	GLuint TEX[64];
	u32 TEXIndex;
};

void GLInitRenderer();

static void GLCreateProgram(GLuint *Program, read_file_result *VsFile, read_file_result *FsFile);

static void GLInitTextureQuadBatch();
static void GLInitColorQuadBatch();
static void GLInitTextBatch();

u32  GLUploadTexture(texture *Texture);
void GLClear();
void GLDrawTextureQuads(void *Data, u32 Count, texture *Texture);
void GLDrawTextureQuads(void *Data, u32 Count, u32 TextureIndex);
void GLDrawColorQuads(void *Data, u32 Count);
void GLDrawText(void *Data, u32 CharCount, texture *FontTexture);
void GLDrawText(void *Data, u32 CharCount, u32 TextureIndex);

#endif