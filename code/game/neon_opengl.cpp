#include "neon_opengl.h"

#include "neon_renderer.h"
#include "neon_texture.h"
#include <imgui.h>

#define NEON_INIT_GL
#define NEON_DEBUG_GL
#include "neon_GL.h"

static render_state RenderState = {};

//-----------------------------------------------------------------------------
// gl helper functions
//-----------------------------------------------------------------------------

void ogl::InitState()
{
	RenderState.TextureCount = 0;
	RenderState.VertBuffCount = 0;
	RenderState.IdxBuffCount = 0;
	RenderState.ShaderProgCount = 0;

	RenderState.OrthoProjection = Orthographic(0.0f, (r32)Platform->Width, (r32)Platform->Height, 0.0f, -1.0f, 1.0f);


	// Set clear color
	glClearColor(0.1f, 0.2f, 0.3f, 1.0f);

	// Set the viewport
	glViewport(0, 0, Platform->Width, Platform->Height);

	// Enable depth testing
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// Set CCW vertex winding as triangle's front.
	glFrontFace(GL_CCW);

	// Enable back-face culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	// Enable the alpha blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Enable SRGB framebuffer
	// This converts fragments shader linear output to gamma correct output i.e. apply pow(1/2.2) to the colors
	// @NOTE: fragment shader must always output in linear color space
	// TODO: Check if SRGB framebuffer is supported.
	glEnable(GL_FRAMEBUFFER_SRGB);

	// Wireframe rendering mode for debugging.
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void ogl::Clear()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

render_resource ogl::MakeTexture(texture *Texture)
{
	if(!Texture->IsValid())
	{
		// TODO: Return RenderResource of the default texture and log this error.
	}

	assert(RenderState.TextureCount < ArrayCount(RenderState.Texture));

	render_resource RenderResource;
	RenderResource.Type = render_resource::TEXTURE;
	RenderResource.ResourceHandle = RenderState.TextureCount++;

	// Generate and bind Texture
	glGenTextures(1, &RenderState.Texture[RenderResource.ResourceHandle]);
	// TODO: Add more texture type in future.
	assert(Texture->Type == texture_type::TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, RenderState.Texture[RenderResource.ResourceHandle]);

	// Orient the texture
	if(!Texture->FlippedAroundY)
		Texture->FlipAroundY();

	// Upload the texture to the GPU
	glTexImage2D(GL_TEXTURE_2D, 0, Texture->HwGammaCorrection ? GL_SRGB_ALPHA : GL_RGBA, Texture->Width, Texture->Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, Texture->Content);

	// Set texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, Texture->Wrap == texture_wrap::REPEAT ? GL_REPEAT : GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, Texture->Wrap == texture_wrap::REPEAT ? GL_REPEAT : GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, Texture->Filter == texture_filter::NEAREST ? GL_NEAREST : GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, Texture->Filter == texture_filter::NEAREST ? GL_NEAREST : GL_LINEAR);

	// Unbind texture
	glBindTexture(GL_TEXTURE_2D, 0);

	return RenderResource;
}

render_resource ogl::MakeVertexBuffer(u32 Size, bool Dynamic)
{
	render_resource RenderResource;

	RenderResource.Type = render_resource::VERTEX_BUFFER;
	RenderResource.ResourceHandle = RenderState.VertBuffCount++;

	GLuint &VertexBuffer = RenderState.VertBuff[RenderResource.ResourceHandle];

	glGenBuffers(1, &VertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, Size, nullptr, Dynamic ? GL_STREAM_DRAW : GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	return RenderResource;
}

void ogl::VertexBufferData(render_resource VertexBuffer, u32 Offset, u32 Size, void const *Data)
{
	GLuint &Buffer = RenderState.VertBuff[VertexBuffer.ResourceHandle];

	glBindBuffer(GL_ARRAY_BUFFER, Buffer);
	glBufferSubData(GL_ARRAY_BUFFER, Offset, Size, Data);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ogl::DeleteVertexBuffer(render_resource VertexBuffer)
{
	glDeleteBuffers(1, &RenderState.VertBuff[VertexBuffer.ResourceHandle]);
}

render_resource ogl::MakeIndexBuffer(u32 Size, bool Dynamic)
{
	render_resource RenderResource;
	
	RenderResource.Type = render_resource::INDEX_BUFFER;
	RenderResource.ResourceHandle = RenderState.IdxBuffCount++;
	
	GLuint &IndexBuffer = RenderState.IdxBuff[RenderResource.ResourceHandle];

	glGenBuffers(1, &IndexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, Size, nullptr, Dynamic ? GL_STREAM_DRAW : GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	return RenderResource;
}

void ogl::IndexBufferData(render_resource IndexBuffer, u32 Offset, u32 Size, void const *Data)
{
	GLuint &Buffer = RenderState.IdxBuff[IndexBuffer.ResourceHandle];

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Buffer);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, Offset, Size, Data);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void ogl::DeleteIndexBuffer(render_resource IndexBuffer)
{
	glDeleteBuffers(1, &RenderState.IdxBuff[IndexBuffer.ResourceHandle]);
}

render_resource ogl::MakeShaderProgram(char const *VertShaderSrc, char const *FragShaderSrc)
{
	render_resource RenderResource;
	RenderResource.Type = render_resource::SHADER_PROGRAM;
	RenderResource.ResourceHandle = RenderState.ShaderProgCount++;

	read_file_result VsFile = Platform->ReadFile(VertShaderSrc);
	read_file_result FsFile = Platform->ReadFile(FragShaderSrc);
	
	GLuint Vs = glCreateShader(GL_VERTEX_SHADER);
	GLuint Fs = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(Vs, 1, (GLchar * const *)(&VsFile.Content), (const GLint *)(&VsFile.ContentSize));
	glShaderSource(Fs, 1, (GLchar * const *)(&FsFile.Content), (const GLint *)(&FsFile.ContentSize));

	Platform->FreeFileMemory(&VsFile);
	Platform->FreeFileMemory(&FsFile);

	glCompileShader(Vs);
	glCompileShader(Fs);

	GLuint &Program = RenderState.ShaderProg[RenderResource.ResourceHandle];

	Program = glCreateProgram();
	glAttachShader(Program, Vs);
	glAttachShader(Program, Fs);

	glLinkProgram(Program);

	glValidateProgram(Program);
	GLint Validated;
	glGetProgramiv(Program, GL_VALIDATE_STATUS, &Validated);

	if(!Validated)
	{
		char VsErrors[8192];
		char FsErrors[8192];
		char ProgramErrors[8192];
		glGetShaderInfoLog(Vs, sizeof(VsErrors), 0, VsErrors);
		glGetShaderInfoLog(Fs, sizeof(FsErrors), 0, FsErrors);
		glGetProgramInfoLog(Program, sizeof(ProgramErrors), 0, ProgramErrors);

		assert(!"Shader compilation and/or linking failed");
	}

	glDetachShader(Program, Vs);
	glDetachShader(Program, Fs);

	glDeleteShader(Vs);
	glDeleteShader(Fs);

	return RenderResource;
}

void ogl::DeleteShaderProgram(render_resource ShaderProgram)
{
	glDeleteProgram(RenderState.ShaderProg[ShaderProgram.ResourceHandle]);
}

void ogl::UnindexedDraw(cmd::udraw *Cmd)
{
	glUseProgram(RenderState.ShaderProg[Cmd->ShaderProgram.ResourceHandle]);

	glBindBuffer(GL_ARRAY_BUFFER, RenderState.VertBuff[Cmd->VertexBuffer.ResourceHandle]);

	if(Cmd->VertexFormat == vert_format::POS3UV2COLOR4)
	{
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vert_POS3UV2COLOR4), (void *)OffsetOf(vert_POS3UV2COLOR4, Position));
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vert_POS3UV2COLOR4), (void *)OffsetOf(vert_POS3UV2COLOR4, UV));
		glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(vert_POS3UV2COLOR4), (void *)OffsetOf(vert_POS3UV2COLOR4, Color));

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, RenderState.Texture[Cmd->Texture->RenderResource.ResourceHandle]);

		GLint loc = glGetUniformLocation(RenderState.ShaderProg[Cmd->ShaderProgram.ResourceHandle], "Projection");
		glUniformMatrix4fv(loc, 1, GL_FALSE, RenderState.OrthoProjection.Elements);
	}

	glDrawArrays(GL_TRIANGLES, Cmd->StartVertex, Cmd->VertexCount);

	glDisableVertexAttribArray(0);
}

/*
static gl_batch TextureQuadBatch;
static gl_batch ColorQuadBatch;
static gl_batch TextBatch;
static gl_batch LineBatch;
static gl_resources GLResources;
*/

/*
void GLInitRenderer()
{
	// Set clear color
	glClearColor(0.08f, 0.08f, 0.61f, 1.0f);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// Set the viewport
	glViewport(0, 0, Platform->Width, Platform->Height);
	
	// Enable depth testing
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// Since we are following the left-handed coodinate system we set
	// clock-wise vertex winding as the triangle's front.
	glFrontFace(GL_CCW);

	// Enable back-face culling to improve performance.
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	
	// Enable the blending to support alpha textures. 
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Enable SRGB framebuffer
	// Convert fragments shader linear output to gamma correct output [pow(1/2.2)]
	// @NOTE: fragment shader must always output in linear color space
	glEnable(GL_FRAMEBUFFER_SRGB); 

	// Wireframe rendering mode for debugging.
	 //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// Init GLResources
	GLResources.TextureIndex = 0;
	GLResources.ShaderProgramIndex = 0;
	GLResources.RenderTargetIndex = 0;
	GLResources.State.BoundRenderTarget = 0;

	mat4 HalfPixelOffset = mat4::Translate(0.5f / Platform->Width, 0.5f / Platform->Height, 0);
	mat4 Projection = mat4::Orthographic(0.0f, (r32)Platform->Width, (r32)Platform->Height, 0.0f, -1.0f, 1.0f);
	GLResources.Camera.CamMatrix = Projection*HalfPixelOffset;

	// Init batches
	GLInitTextureQuadBatch();
	GLInitColorQuadBatch();
	GLInitTextBatch();
	GLInitLineBatch();
}

u32 GLCreateProgram(read_file_result *VsFile, read_file_result *FsFile)
{
	GLuint Vs = glCreateShader(GL_VERTEX_SHADER);
	GLuint Fs = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(Vs, 1, (GLchar * const *)(&VsFile->Content), (const GLint *)(&VsFile->ContentSize));
	glShaderSource(Fs, 1, (GLchar * const *)(&FsFile->Content), (const GLint *)(&FsFile->ContentSize));

	glCompileShader(Vs);
	glCompileShader(Fs);

	GLuint & Program = GLResources.ShaderProgram[GLResources.ShaderProgramIndex].PROGHandle;

	Program = glCreateProgram();
	glAttachShader(Program, Vs);
	glAttachShader(Program, Fs);

	glLinkProgram(Program);
		
	glValidateProgram(Program);
	GLint Validated;
	glGetProgramiv(Program, GL_VALIDATE_STATUS, &Validated);
	
	if(!Validated)
	{
		char VsErrors[8192];
		char FsErrors[8192];
		char ProgramErrors[8192];
		glGetShaderInfoLog(Vs, sizeof(VsErrors), 0, VsErrors);
		glGetShaderInfoLog(Fs, sizeof(FsErrors), 0, FsErrors);
		glGetProgramInfoLog(Program, sizeof(ProgramErrors), 0, ProgramErrors);

		Assert(!"Shader compilation and/or linking failed");	
	}	

	glDetachShader(Program, Vs);
	glDetachShader(Program, Fs);

	glDeleteShader(Vs);
	glDeleteShader(Fs);

	return GLResources.ShaderProgramIndex++;
}

static
void GLInitTextureQuadBatch()
{
	// Create shader program
	read_file_result VertShdr, FragShdr;

	VertShdr = Platform->ReadFile("shader/texture_quad.vs");
	FragShdr = Platform->ReadFile("shader/texture_quad.fs");

	TextureQuadBatch.PROGIndex = GLCreateProgram(&VertShdr, &FragShdr);

	Platform->FreeFileMemory(&VertShdr);
	Platform->FreeFileMemory(&FragShdr);

	// Allocate buffer
	GLuint PosLoc, ColorLoc, TexCoordLoc, map0Loc, ProjMatLoc;

	glGenBuffers(1, &TextureQuadBatch.VBO);
	glBindBuffer(GL_ARRAY_BUFFER, TextureQuadBatch.VBO);

	glBufferData(GL_ARRAY_BUFFER, MEGABYTE(8), 0, GL_DYNAMIC_DRAW);

	glGenVertexArrays(1, &TextureQuadBatch.VAO);
	glBindVertexArray(TextureQuadBatch.VAO);

	GLuint & Program = GLResources.ShaderProgram[TextureQuadBatch.PROGIndex].PROGHandle;

	PosLoc = glGetAttribLocation(Program, "vs_pos");
	Assert(PosLoc != -1);
	ColorLoc = glGetAttribLocation(Program, "vs_color");
	Assert(ColorLoc != -1);
	TexCoordLoc = glGetAttribLocation(Program, "vs_texcoord");
	Assert(TexCoordLoc != -1);

	map0Loc = glGetUniformLocation(Program, "map0");
	Assert(map0Loc != -1);
	TextureQuadBatch.map0Uniform = map0Loc;

	ProjMatLoc = glGetUniformLocation(Program, "projection");
	Assert(ProjMatLoc != -1);
	TextureQuadBatch.projectionUniform = ProjMatLoc; 

	glVertexAttribPointer(PosLoc, 3, GL_FLOAT, GL_FALSE, 36, (void *)0);
	glVertexAttribPointer(ColorLoc, 4, GL_FLOAT, GL_FALSE, 36, (void *)12);
	glVertexAttribPointer(TexCoordLoc, 2, GL_FLOAT, GL_FALSE, 36, (void *)28);
	glEnableVertexAttribArray(PosLoc);
	glEnableVertexAttribArray(ColorLoc);
	glEnableVertexAttribArray(TexCoordLoc);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER,0);
}

static
void GLInitColorQuadBatch()
{
	// Create shader
	read_file_result VertShdr, FragShdr;

	VertShdr = Platform->ReadFile("shader/color_quad.vs");
	FragShdr = Platform->ReadFile("shader/color_quad.fs");

	ColorQuadBatch.PROGIndex = GLCreateProgram(&VertShdr, &FragShdr);

	Platform->FreeFileMemory(&VertShdr);
	Platform->FreeFileMemory(&FragShdr);

	// Allocate buffer
	GLuint PosLoc, ColorLoc, ProjMatLoc;

	glGenBuffers(1, &ColorQuadBatch.VBO);
	glBindBuffer(GL_ARRAY_BUFFER, ColorQuadBatch.VBO);

	glBufferData(GL_ARRAY_BUFFER, MEGABYTE(8), 0, GL_DYNAMIC_DRAW);

	glGenVertexArrays(1, &ColorQuadBatch.VAO);
	glBindVertexArray(ColorQuadBatch.VAO);

	GLuint & Program = GLResources.ShaderProgram[ColorQuadBatch.PROGIndex].PROGHandle;

	PosLoc = glGetAttribLocation(Program, "vs_pos");
	Assert(PosLoc != -1);
	ColorLoc = glGetAttribLocation(Program, "vs_color");
	Assert(ColorLoc != -1);

	ProjMatLoc = glGetUniformLocation(Program, "projection");
	Assert(ProjMatLoc != -1);
	ColorQuadBatch.projectionUniform = ProjMatLoc; 

	glVertexAttribPointer(PosLoc, 3, GL_FLOAT, GL_FALSE, 28, (void *)0);
	glVertexAttribPointer(ColorLoc, 4, GL_FLOAT, GL_FALSE, 28, (void *)12);
	glEnableVertexAttribArray(PosLoc);
	glEnableVertexAttribArray(ColorLoc);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER,0);
}

static
void GLInitTextBatch()
{
	// Create shader
	read_file_result VertShdr, FragShdr;

	VertShdr = Platform->ReadFile("shader/text.vs");
	FragShdr = Platform->ReadFile("shader/text.fs");

	TextBatch.PROGIndex = GLCreateProgram(&VertShdr, &FragShdr);

	Platform->FreeFileMemory(&VertShdr);
	Platform->FreeFileMemory(&FragShdr);

	// Create vertex buffers
	GLuint PosLoc, ColorLoc, TexCoordLoc, map0Loc, ProjMatLoc;

	glGenBuffers(1, &TextBatch.VBO);
	glBindBuffer(GL_ARRAY_BUFFER, TextBatch.VBO);

	glBufferData(GL_ARRAY_BUFFER, MEGABYTE(8), 0, GL_DYNAMIC_DRAW);

	glGenVertexArrays(1, &TextBatch.VAO);
	glBindVertexArray(TextBatch.VAO);

	GLuint & Program = GLResources.ShaderProgram[TextBatch.PROGIndex].PROGHandle;

	PosLoc = glGetAttribLocation(Program, "vs_pos");
	Assert(PosLoc != -1);
	ColorLoc = glGetAttribLocation(Program, "vs_color");
	Assert(ColorLoc != -1);
	TexCoordLoc = glGetAttribLocation(Program, "vs_texcoord");
	Assert(TexCoordLoc != -1);

	map0Loc = glGetUniformLocation(Program, "map0");
	Assert(map0Loc != -1);
	TextBatch.map0Uniform = map0Loc;

	ProjMatLoc = glGetUniformLocation(Program, "projection");
	Assert(ProjMatLoc != -1);
	TextBatch.projectionUniform = ProjMatLoc; 

	glVertexAttribPointer(PosLoc, 3, GL_FLOAT, GL_FALSE, 36, (void *)0);
	glVertexAttribPointer(ColorLoc, 4, GL_FLOAT, GL_FALSE, 36, (void *)12);
	glVertexAttribPointer(TexCoordLoc, 2, GL_FLOAT, GL_FALSE, 36, (void *)28);
	glEnableVertexAttribArray(PosLoc);
	glEnableVertexAttribArray(ColorLoc);
	glEnableVertexAttribArray(TexCoordLoc);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER,0);
}

static
void GLInitLineBatch()
{
	// Create shader
	read_file_result VertShdr, FragShdr;

	VertShdr = Platform->ReadFile("shader/line.vs");
	FragShdr = Platform->ReadFile("shader/line.fs");

	LineBatch.PROGIndex = GLCreateProgram(&VertShdr, &FragShdr);

	Platform->FreeFileMemory(&VertShdr);
	Platform->FreeFileMemory(&FragShdr);

	// Allocate buffer
	GLuint PosLoc, ColorLoc, ProjMatLoc;

	glGenBuffers(1, &LineBatch.VBO);
	glBindBuffer(GL_ARRAY_BUFFER, LineBatch.VBO);

	glBufferData(GL_ARRAY_BUFFER, MEGABYTE(8), 0, GL_DYNAMIC_DRAW);

	glGenVertexArrays(1, &LineBatch.VAO);
	glBindVertexArray(LineBatch.VAO);

	GLuint & Program = GLResources.ShaderProgram[LineBatch.PROGIndex].PROGHandle;

	PosLoc = glGetAttribLocation(Program, "vs_pos");
	Assert(PosLoc != -1);
	ColorLoc = glGetAttribLocation(Program, "vs_color");
	Assert(ColorLoc != -1);

	ProjMatLoc = glGetUniformLocation(Program, "projection");
	Assert(ProjMatLoc != -1);
	LineBatch.projectionUniform = ProjMatLoc; 

	glVertexAttribPointer(PosLoc, 3, GL_FLOAT, GL_FALSE, 28, (void *)0);
	glVertexAttribPointer(ColorLoc, 4, GL_FLOAT, GL_FALSE, 28, (void *)12);
	glEnableVertexAttribArray(PosLoc);
	glEnableVertexAttribArray(ColorLoc);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER,0);
}

u32 GLUploadTexture(texture *Texture, texture_param Target, texture_param Filter,
					texture_param Wrap)
{
	Assert(GLResources.TextureIndex < ArrayCount(GLResources.Texture));

	GLenum GLTarget = 0, GLFilter = 0, GLWrap = 0;

	if(Target == TEXTURE_2D)
	{
		GLTarget = GL_TEXTURE_2D;
	}
	else
	{
		Assert(!"Invalid texture target.");
	}

	if(Filter == NEAREST_FILTER)
	{
		GLFilter = GL_NEAREST;
	}
	else if(Filter == LINEAR_FILTER)
	{
		GLFilter = GL_LINEAR;
	}
	else
	{
		Assert(!"Invalid texture filter parameter.");
	}

	if(Wrap == WRAP_REPEAT)
	{
		GLWrap = GL_REPEAT;
	}
	else if(Wrap == WRAP_CLAMP)
	{
		GLWrap = GL_CLAMP_TO_EDGE;
	}
	else
	{
		Assert(!"Invalid texture wrap parameter.");
	}

	// Generate and bind Texture
	glGenTextures(1, &GLResources.Texture[GLResources.TextureIndex].TEXHandle);
	glBindTexture(GLTarget, GLResources.Texture[GLResources.TextureIndex].TEXHandle);

	// Orient the texture
	if(!Texture->FlippedAroundY)
	{
		//Texture->FlipVertically();
		Texture->FlipAroundY();
	}

	// Upload the texture to the GPU
	glTexImage2D(GLTarget, 0, GL_SRGB_ALPHA, Texture->Width, Texture->Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, Texture->Content);
	
	// Set texture parameters
	glTexParameteri(GLTarget, GL_TEXTURE_WRAP_S, GLWrap);
	glTexParameteri(GLTarget, GL_TEXTURE_WRAP_T, GLWrap);
	glTexParameteri(GLTarget, GL_TEXTURE_MIN_FILTER, GLFilter);
	glTexParameteri(GLTarget, GL_TEXTURE_MAG_FILTER, GLFilter);

	// Unbind texture
	glBindTexture(GLTarget, 0);

	return GLResources.TextureIndex++;
}

u32 GLDeleteTexture()
{
	return 0;
}

render_target GLCreateRenderTarget(ivec2 Size, texture_param Filter)
{
	render_target Result;
	Result.Size = Size;

	Assert(GLResources.RenderTargetIndex < ArrayCount(GLResources.RenderTarget));

	u32 TargetIndex = ++GLResources.RenderTargetIndex;

	GLenum GLFilter = 0;
	if(Filter ==NEAREST_FILTER)
	{
		GLFilter = GL_NEAREST;
	}
	else if(Filter == LINEAR_FILTER)
	{
		GLFilter = GL_LINEAR;
	}
	else
	{
		Assert(!"Invalid texture filter parameter.");
	}

	GLuint& FBOHandle = GLResources.RenderTarget[TargetIndex].FBOHandle;
	GLuint& ColorTEXHandle = GLResources.RenderTarget[TargetIndex].ColorTEXHandle;
	GLuint& DepthRBOHandle = GLResources.RenderTarget[TargetIndex].DepthRBOHandle;

	// Generate & bind FBO
	glGenFramebuffers(1, &FBOHandle);
	glBindFramebuffer(GL_FRAMEBUFFER, FBOHandle);

	// Generate and bind ColorTexture
	glGenTextures(1, &ColorTEXHandle);
	glBindTexture(GL_TEXTURE_2D, ColorTEXHandle);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, Size.x, Size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GLFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GLFilter);

	// Attach ColorTexture to FBO
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ColorTEXHandle, 0);

	// Unbind ColorTexture 
	glBindTexture(GL_TEXTURE_2D, 0);

	// Generate and bind DepthRBO
	glGenRenderbuffers(1, &DepthRBOHandle);
	glBindRenderbuffer(GL_RENDERBUFFER, DepthRBOHandle);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, Size.x, Size.y);

	// Attach DepthRBO to FBO
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, DepthRBOHandle);

	// Unbind DepthRBO
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	// Check if framebuffer is complete
	Assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Bind default framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	Result.Handle = TargetIndex;
	return Result;
}

u32 GLDeleteRenderTarget()
{
	return 0;
}

void GLClear()
{	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GLDrawLines(void *Data, u32 Count, u8 RenderTargetIndex)
{
	if(GLResources.State.BoundRenderTarget != RenderTargetIndex)
	{
		if(RenderTargetIndex == 0)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
		else
		{
			glBindFramebuffer(GL_FRAMEBUFFER, GLResources.RenderTarget[RenderTargetIndex].FBOHandle);
		}
		GLResources.State.BoundRenderTarget = RenderTargetIndex;
	}

	glUseProgram(GLResources.ShaderProgram[LineBatch.PROGIndex].PROGHandle);

	glBindBuffer(GL_ARRAY_BUFFER, LineBatch.VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 14 * sizeof(GLfloat) * Count, Data);

	glBindVertexArray(LineBatch.VAO);

	glUniformMatrix4fv(LineBatch.projectionUniform, 1, GL_TRUE, GLResources.Camera.CamMatrix.Elements);
	
	glDrawArrays(GL_LINES, 0, Count * 2);

	glBindVertexArray(0);
	glUseProgram(0);
}

void GLDrawTextureQuads(void *Data, u32 Count, u32 TextureIndex, u8 RenderTargetIndex)
{
	if(GLResources.State.BoundRenderTarget != RenderTargetIndex)
	{
		if(RenderTargetIndex == 0)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
		else
		{
			glBindFramebuffer(GL_FRAMEBUFFER, GLResources.RenderTarget[RenderTargetIndex].FBOHandle);
		}
		GLResources.State.BoundRenderTarget = RenderTargetIndex;
	}

	glUseProgram(GLResources.ShaderProgram[TextureQuadBatch.PROGIndex].PROGHandle);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, GLResources.Texture[TextureIndex].TEXHandle);
	glUniform1i(TextureQuadBatch.map0Uniform, 0);

	glBindBuffer(GL_ARRAY_BUFFER, TextureQuadBatch.VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 54 * Count * sizeof(GLfloat), Data);

	glBindVertexArray(TextureQuadBatch.VAO);

	glUniformMatrix4fv(TextureQuadBatch.projectionUniform, 1, GL_TRUE, GLResources.Camera.CamMatrix.Elements);
	
	glDrawArrays(GL_TRIANGLES, 0, Count * 6);

	glBindVertexArray(0);
	glUseProgram(0);
}

void GLDrawText(void *Data, u32 CharCount, u32 TextureIndex, u8 RenderTargetIndex)
{
	if(GLResources.State.BoundRenderTarget != RenderTargetIndex)
	{
		if(RenderTargetIndex == 0)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
		else
		{
			glBindFramebuffer(GL_FRAMEBUFFER, GLResources.RenderTarget[RenderTargetIndex].FBOHandle);
		}
		GLResources.State.BoundRenderTarget = RenderTargetIndex;
	}

	glUseProgram(GLResources.ShaderProgram[TextBatch.PROGIndex].PROGHandle);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, GLResources.Texture[TextureIndex].TEXHandle);
	glUniform1i(TextBatch.map0Uniform, 0);

	glBindBuffer(GL_ARRAY_BUFFER, TextBatch.VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 54 * CharCount * sizeof(GLfloat), Data);

	glBindVertexArray(TextBatch.VAO);

	glUniformMatrix4fv(TextBatch.projectionUniform, 1, GL_TRUE, GLResources.Camera.CamMatrix.Elements);
	
	glDrawArrays(GL_TRIANGLES, 0, CharCount * 6);

	glBindVertexArray(0);
	glUseProgram(0);
}


void GLDrawColorQuads(void *Data, u32 Count, u8 RenderTargetIndex)
{
	if(GLResources.State.BoundRenderTarget != RenderTargetIndex)
	{
		if(RenderTargetIndex == 0)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
		else
		{
			glBindFramebuffer(GL_FRAMEBUFFER, GLResources.RenderTarget[RenderTargetIndex].FBOHandle);
		}
		GLResources.State.BoundRenderTarget = RenderTargetIndex;
	}

	glUseProgram(GLResources.ShaderProgram[ColorQuadBatch.PROGIndex].PROGHandle);

	glBindBuffer(GL_ARRAY_BUFFER, ColorQuadBatch.VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 42 * Count * sizeof(GLfloat), Data);

	glBindVertexArray(ColorQuadBatch.VAO);

	glUniformMatrix4fv(ColorQuadBatch.projectionUniform, 1, GL_TRUE, GLResources.Camera.CamMatrix.Elements);
	
	glDrawArrays(GL_TRIANGLES, 0, Count * 6);

	glBindVertexArray(0);
	glUseProgram(0);
}

void GLDrawRenderTarget(void *Data, render_target SrcRenderTarget, render_target DestRenderTarget)
{
	if(GLResources.State.BoundRenderTarget != DestRenderTarget.Handle)
	{
		if(DestRenderTarget.Handle == 0)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
		else
		{
			glBindFramebuffer(GL_FRAMEBUFFER, GLResources.RenderTarget[DestRenderTarget.Handle].FBOHandle);
		}
		GLResources.State.BoundRenderTarget = DestRenderTarget.Handle;
	}

	glUseProgram(GLResources.ShaderProgram[TextureQuadBatch.PROGIndex].PROGHandle);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, GLResources.RenderTarget[SrcRenderTarget.Handle].ColorTEXHandle);
	glUniform1i(TextureQuadBatch.map0Uniform, 0);

	glBindBuffer(GL_ARRAY_BUFFER, TextureQuadBatch.VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 54 * sizeof(GLfloat), Data);

	glBindVertexArray(TextureQuadBatch.VAO);

	glUniformMatrix4fv(TextureQuadBatch.projectionUniform, 1, GL_TRUE, GLResources.Camera.CamMatrix.Elements);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glBindVertexArray(0);
	glUseProgram(0);
}
*/

//-----------------------------------------------------------------------------
// ImGui Rendering
//-----------------------------------------------------------------------------

// ImGui Data
static GLuint       imgui_FontTexture = 0;
static int          imgui_ShaderHandle = 0, imgui_VertHandle = 0, imgui_FragHandle = 0;
static int          imgui_AttribLocationTex = 0, imgui_AttribLocationProjMtx = 0;
static int          imgui_AttribLocationPosition = 0, imgui_AttribLocationUV = 0, imgui_AttribLocationColor = 0;
static unsigned int imgui_VboHandle = 0, imgui_VaoHandle = 0, imgui_ElementsHandle = 0;

extern "C" DLLEXPORT
IMGUI_RENDER_DRAW_LISTS(ImGui_RenderDrawLists)
{
	// Avoid rendering when minimized, scale coordinates for retina displays (screen coordinates != framebuffer coordinates)
    ImGuiIO& io = ImGui::GetIO();
    int fb_width = (int)(io.DisplaySize.x * io.DisplayFramebufferScale.x);
    int fb_height = (int)(io.DisplaySize.y * io.DisplayFramebufferScale.y);
    if (fb_width == 0 || fb_height == 0)
        return;
    draw_data->ScaleClipRects(io.DisplayFramebufferScale);

    // Backup GL state
    GLenum last_active_texture; glGetIntegerv(GL_ACTIVE_TEXTURE, (GLint*)&last_active_texture);
    glActiveTexture(GL_TEXTURE0);
    GLint last_program; glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
    GLint last_texture; glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
    GLint last_sampler; glGetIntegerv(GL_SAMPLER_BINDING, &last_sampler);
    GLint last_array_buffer; glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
    GLint last_element_array_buffer; glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &last_element_array_buffer);
    GLint last_vertex_array; glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vertex_array);
    GLint last_polygon_mode[2]; glGetIntegerv(GL_POLYGON_MODE, last_polygon_mode);
    GLint last_viewport[4]; glGetIntegerv(GL_VIEWPORT, last_viewport);
    GLint last_scissor_box[4]; glGetIntegerv(GL_SCISSOR_BOX, last_scissor_box);
    GLenum last_blend_src_rgb; glGetIntegerv(GL_BLEND_SRC_RGB, (GLint*)&last_blend_src_rgb);
    GLenum last_blend_dst_rgb; glGetIntegerv(GL_BLEND_DST_RGB, (GLint*)&last_blend_dst_rgb);
    GLenum last_blend_src_alpha; glGetIntegerv(GL_BLEND_SRC_ALPHA, (GLint*)&last_blend_src_alpha);
    GLenum last_blend_dst_alpha; glGetIntegerv(GL_BLEND_DST_ALPHA, (GLint*)&last_blend_dst_alpha);
    GLenum last_blend_equation_rgb; glGetIntegerv(GL_BLEND_EQUATION_RGB, (GLint*)&last_blend_equation_rgb);
    GLenum last_blend_equation_alpha; glGetIntegerv(GL_BLEND_EQUATION_ALPHA, (GLint*)&last_blend_equation_alpha);
    GLboolean last_enable_blend = glIsEnabled(GL_BLEND);
    GLboolean last_enable_cull_face = glIsEnabled(GL_CULL_FACE);
    GLboolean last_enable_depth_test = glIsEnabled(GL_DEPTH_TEST);
    GLboolean last_enable_scissor_test = glIsEnabled(GL_SCISSOR_TEST);

    // Setup render state: alpha-blending enabled, no face culling, no depth testing, scissor enabled, polygon fill
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_SCISSOR_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // Setup viewport, orthographic projection matrix
    glViewport(0, 0, (GLsizei)fb_width, (GLsizei)fb_height);
    const float ortho_projection[4][4] =
    {
        { 2.0f/io.DisplaySize.x, 0.0f,                   0.0f, 0.0f },
        { 0.0f,                  2.0f/-io.DisplaySize.y, 0.0f, 0.0f },
        { 0.0f,                  0.0f,                  -1.0f, 0.0f },
        {-1.0f,                  1.0f,                   0.0f, 1.0f },
    };
    glUseProgram(imgui_ShaderHandle);
    glUniform1i(imgui_AttribLocationTex, 0);
    glUniformMatrix4fv(imgui_AttribLocationProjMtx, 1, GL_FALSE, &ortho_projection[0][0]);
    glBindVertexArray(imgui_VaoHandle);
    glBindSampler(0, 0); // Rely on combined texture/sampler state.

    for (int n = 0; n < draw_data->CmdListsCount; n++)
    {
        const ImDrawList* cmd_list = draw_data->CmdLists[n];
        const ImDrawIdx* idx_buffer_offset = 0;

        glBindBuffer(GL_ARRAY_BUFFER, imgui_VboHandle);
        glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)cmd_list->VtxBuffer.Size * sizeof(ImDrawVert), (const GLvoid*)cmd_list->VtxBuffer.Data, GL_STREAM_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, imgui_ElementsHandle);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx), (const GLvoid*)cmd_list->IdxBuffer.Data, GL_STREAM_DRAW);

        for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
        {
            const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
            if (pcmd->UserCallback)
            {
                pcmd->UserCallback(cmd_list, pcmd);
            }
            else
            {
                glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)pcmd->TextureId);
                glScissor((int)pcmd->ClipRect.x, (int)(fb_height - pcmd->ClipRect.w), (int)(pcmd->ClipRect.z - pcmd->ClipRect.x), (int)(pcmd->ClipRect.w - pcmd->ClipRect.y));
                glDrawElements(GL_TRIANGLES, (GLsizei)pcmd->ElemCount, sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, idx_buffer_offset);
            }
            idx_buffer_offset += pcmd->ElemCount;
        }
    }

    // Restore modified GL state
    glUseProgram(last_program);
    glBindTexture(GL_TEXTURE_2D, last_texture);
    glBindSampler(0, last_sampler);
    glActiveTexture(last_active_texture);
    glBindVertexArray(last_vertex_array);
    glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, last_element_array_buffer);
    glBlendEquationSeparate(last_blend_equation_rgb, last_blend_equation_alpha);
    glBlendFuncSeparate(last_blend_src_rgb, last_blend_dst_rgb, last_blend_src_alpha, last_blend_dst_alpha);
    if (last_enable_blend) glEnable(GL_BLEND); else glDisable(GL_BLEND);
    if (last_enable_cull_face) glEnable(GL_CULL_FACE); else glDisable(GL_CULL_FACE);
    if (last_enable_depth_test) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
    if (last_enable_scissor_test) glEnable(GL_SCISSOR_TEST); else glDisable(GL_SCISSOR_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, last_polygon_mode[0]);
    glViewport(last_viewport[0], last_viewport[1], (GLsizei)last_viewport[2], (GLsizei)last_viewport[3]);
    glScissor(last_scissor_box[0], last_scissor_box[1], (GLsizei)last_scissor_box[2], (GLsizei)last_scissor_box[3]);
}

static void ImGui_CreateFontsTexture()
{
	// Build texture atlas
    ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontFromFileTTF("font/DroidSans.ttf", 16);

    unsigned char* pixels;
    int width, height;
    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);   // Load as RGBA 32-bits for OpenGL3 demo because it is more likely to be compatible with user's existing shader.

    // Upload texture to graphics system
    GLint last_texture;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
    glGenTextures(1, &imgui_FontTexture);
    glBindTexture(GL_TEXTURE_2D, imgui_FontTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    // Store our identifier
    io.Fonts->TexID = (void *)(intptr_t)imgui_FontTexture;

    // Restore state
    glBindTexture(GL_TEXTURE_2D, last_texture);
}

DLLEXPORT
IMGUI_CREATE_DEVICE_OBJECTS(ImGui_CreateDeviceObjects)
{
	// Backup GL state
    GLint last_texture, last_array_buffer, last_vertex_array;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vertex_array);

    const GLchar *vertex_shader =
        "#version 330\n"
        "uniform mat4 ProjMtx;\n"
        "in vec2 Position;\n"
        "in vec2 UV;\n"
        "in vec4 Color;\n"
        "out vec2 Frag_UV;\n"
        "out vec4 Frag_Color;\n"
        "void main()\n"
        "{\n"
        "	Frag_UV = UV;\n"
        "	Frag_Color = Color;\n"
        "	gl_Position = ProjMtx * vec4(Position.xy,0,1);\n"
        "}\n";

/*    const GLchar* fragment_shader =
        "#version 330\n"
        "uniform sampler2D Texture;\n"
        "in vec2 Frag_UV;\n"
        "in vec4 Frag_Color;\n"
        "out vec4 Out_Color;\n"
        "void main()\n"
        "{\n"
        "	Out_Color = Frag_Color * texture( Texture, Frag_UV.st);\n"
        "}\n";*/
    const GLchar* fragment_shader =
        "#version 330\n"
        "uniform sampler2D Texture;\n"
        "in vec2 Frag_UV;\n"
        "in vec4 Frag_Color;\n"
        "out vec4 Out_Color;\n"
        "void main()\n"
        "{\n"
 		"   vec4 sRGB_Color = Frag_Color * texture( Texture, Frag_UV.st);\n" // Colors already in sRGB color-space.
        "   Out_Color = vec4(pow(sRGB_Color.rgb, vec3(2.2)), sRGB_Color.a);\n" // Apply gamma so that when GL apply sRGB transformation we get correct results.
        "}\n";

    imgui_ShaderHandle = glCreateProgram();
    imgui_VertHandle = glCreateShader(GL_VERTEX_SHADER);
    imgui_FragHandle = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(imgui_VertHandle, 1, &vertex_shader, 0);
    glShaderSource(imgui_FragHandle, 1, &fragment_shader, 0);
    glCompileShader(imgui_VertHandle);
    glCompileShader(imgui_FragHandle);
    glAttachShader(imgui_ShaderHandle, imgui_VertHandle);
    glAttachShader(imgui_ShaderHandle, imgui_FragHandle);
    glLinkProgram(imgui_ShaderHandle);

    imgui_AttribLocationTex = glGetUniformLocation(imgui_ShaderHandle, "Texture");
    imgui_AttribLocationProjMtx = glGetUniformLocation(imgui_ShaderHandle, "ProjMtx");
    imgui_AttribLocationPosition = glGetAttribLocation(imgui_ShaderHandle, "Position");
    imgui_AttribLocationUV = glGetAttribLocation(imgui_ShaderHandle, "UV");
    imgui_AttribLocationColor = glGetAttribLocation(imgui_ShaderHandle, "Color");

    glGenBuffers(1, &imgui_VboHandle);
    glGenBuffers(1, &imgui_ElementsHandle);

    glGenVertexArrays(1, &imgui_VaoHandle);
    glBindVertexArray(imgui_VaoHandle);
    glBindBuffer(GL_ARRAY_BUFFER, imgui_VboHandle);
    glEnableVertexAttribArray(imgui_AttribLocationPosition);
    glEnableVertexAttribArray(imgui_AttribLocationUV);
    glEnableVertexAttribArray(imgui_AttribLocationColor);

#define OFFSETOF(TYPE, ELEMENT) ((size_t)&(((TYPE *)0)->ELEMENT))
    glVertexAttribPointer(imgui_AttribLocationPosition, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)OFFSETOF(ImDrawVert, pos));
    glVertexAttribPointer(imgui_AttribLocationUV, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)OFFSETOF(ImDrawVert, uv));
    glVertexAttribPointer(imgui_AttribLocationColor, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(ImDrawVert), (GLvoid*)OFFSETOF(ImDrawVert, col));
#undef OFFSETOF

    ImGui_CreateFontsTexture();

    // Restore modified GL state
    glBindTexture(GL_TEXTURE_2D, last_texture);
    glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
    glBindVertexArray(last_vertex_array);

    return true;
}

DLLEXPORT
IMGUI_INVALIDATE_DEVICE_OBJECTS(ImGui_InvalidateDeviceObjects)
{
	if (imgui_VaoHandle) glDeleteVertexArrays(1, &imgui_VaoHandle);
    if (imgui_VboHandle) glDeleteBuffers(1, &imgui_VboHandle);
    if (imgui_ElementsHandle) glDeleteBuffers(1, &imgui_ElementsHandle);
    imgui_VaoHandle = imgui_VboHandle = imgui_ElementsHandle = 0;

    if (imgui_ShaderHandle && imgui_VertHandle) glDetachShader(imgui_ShaderHandle, imgui_VertHandle);
    if (imgui_VertHandle) glDeleteShader(imgui_VertHandle);
    imgui_VertHandle = 0;

    if (imgui_ShaderHandle && imgui_FragHandle) glDetachShader(imgui_ShaderHandle, imgui_FragHandle);
    if (imgui_FragHandle) glDeleteShader(imgui_FragHandle);
    imgui_FragHandle = 0;

    if (imgui_ShaderHandle) glDeleteProgram(imgui_ShaderHandle);
    imgui_ShaderHandle = 0;

    if (imgui_FontTexture)
    {
        glDeleteTextures(1, &imgui_FontTexture);
        ImGui::GetIO().Fonts->TexID = 0;
        imgui_FontTexture = 0;
    }
}

/*
void GLDrawDebugAxis()
{
	glDisable(GL_DEPTH_TEST);
	glUseProgram(0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	static r32 Dy = 0.0f, Dx = 0.0f; 
	mat4 LookAt = mat4::LookAt(vec3(Dx,Dy,3.0f), vec3(0,0,0), vec3(0,1,0));
	Dy += 0.001f;
	Dx += 0.001f;
	LookAt = mat4::Transpose(LookAt);
	glLoadMatrixf(LookAt.Elements);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	// mat4 Proj = mat4::Orthographic(-1.0f, 1.0f, 1.0f, -1.0f, 0.1f, 10.0f);
	mat4 Proj = mat4::Perspective(110, 16/9, 0.1f, 100.0f);
	mat4 Proj1 = mat4::Transpose(Proj);
	glLoadMatrixf(Proj1.Elements);

	vec3 Origin(0.0f, 0.0f, 0.0f), WidgetSize(2, 2, 2);
	glBegin(GL_LINES);
		glColor3f(1.0f, 0, 0);
		glVertex3f(Origin.x, Origin.y, Origin.z);
		glVertex3f(Origin.x + WidgetSize.x, Origin.y, Origin.z);

		glColor3f(0, 1.0f, 0);
		glVertex3f(Origin.x, Origin.y, Origin.z);
		glVertex3f(Origin.x, Origin.y + WidgetSize.y, Origin.z);

		glColor3f(0, 0, 1.0f);
		glVertex3f(Origin.x, Origin.y, Origin.z);
		glVertex3f(Origin.x, Origin.y, Origin.z + WidgetSize.z);
	glEnd();
	glEnable(GL_DEPTH_TEST);
}
*/
