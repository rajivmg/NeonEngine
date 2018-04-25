#ifndef NEON_OPENGL_H
#define NEON_OPENGL_H

#include "neon_GL.h"

#include "../platform/neon_platform.h"
#include "neon_math.h"

class texture;
struct render_resource;

namespace cmd
{
	struct draw;
	struct draw_indexed;
}

enum class texture_type;
enum class texture_filter;
enum class texture_wrap;

struct buffer_object
{
	GLuint	Buffer;
	u32		Capacity;
	bool	IsDynamic;
};

struct shader_program
{
	GLuint	Program;
	u32		Sampler2DCount;
	GLint	Sampler2DLoc[10];
	GLint	ProjMatrixLoc;
	GLint	ViewMatrixLoc;
};

// TODO: Reuse same slots after the buffer or texture is deleted.
struct render_state
{
	GLuint			Textures[1024];
	u32				TextureCurrent;
	buffer_object	VertexBuffers[2048];
	u32				VertexBufferCurrent;
	buffer_object	IndexBuffers[2048];
	u32				IndexBufferCurrent;
	shader_program	ShaderPrograms[1024];
	u32				ShaderProgramCurrent;

	u32				ActiveShaderProgram;
};

namespace ogl
{
	void			InitState();
	void			Clear();
	void			SetViewMatrix(mat4 Matrix);
	void			SetProjectionMatrix(mat4 Matrix);
	render_resource MakeTexture(texture *Texture);
	void			DeleteTexture(render_resource Texture);
	render_resource	MakeVertexBuffer(u32 Size, bool Dynamic = true);
	void			VertexBufferData(render_resource VertexBuffer, u32 Offset, u32 Size, void const *Data);
	void			DeleteVertexBuffer(render_resource VertexBuffer);
	render_resource	MakeIndexBuffer(u32 Size, bool Dynamic = true);
	void			IndexBufferData(render_resource IndexBuffer, u32 Offset, u32 Size, void const *Data);
	void			DeleteIndexBuffer(render_resource IndexBuffer);
	render_resource MakeShaderProgram(char const *VertShaderSrc, char const *FragShaderSrc);
	void			DeleteShaderProgram(render_resource ShaderProgram);
	void			UseShaderProgram(render_resource ShaderProgram);
	void			Draw(cmd::draw *Cmd);
	void			DrawIndexed(cmd::draw_indexed *Cmd);
}

#endif