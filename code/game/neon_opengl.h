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
	GLint	Sampler2DLoc[10];
	GLint	ProjMatrixLoc;
};

struct render_state
{
	GLuint			Texture[1024];
	s32				TextureCurrent;
	buffer_object	VertexBuffer[2048];
	s32				VertexBufferCurrent;
	buffer_object	IndexBuffer[2048];
	s32				IndexBufferCurrent;
	shader_program	ShaderProgram[1024];
	s32				ShaderProgramCurrent;

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