#include "neon_opengl.h"

static gl_batch TextureQuadBatch;
static gl_batch ColorQuadBatch;
static gl_batch TextBatch;
static gl_batch LineBatch;

static gl_resources GLResources;

void GLInitRenderer()
{
	// Set clear color
	// glClearColor(0.08f, 0.08f, 0.61f, 1.0f);
	glClearColor(1.0f, 0.08f, 0.61f, 1.0f);


	// set the viewport
	glViewport(0, 0, Platform->Width, Platform->Height);
	
	// enable depth testing
	glEnable(GL_DEPTH_TEST);

	// since we are following the left-handed coodinate system we set
	// clock-wise vertex winding as the triangle's front.
	glFrontFace(GL_CCW);

	// enable back-face culling to improve performance.
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	
	// enable the blending to support alpha textures. 
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// enable SRGB framebuffer
	// convert fragments shader linear output to gamma correct output [pow(1/2.2)]
	// @NOTE: fragment shader must always output in linear color space
	glEnable(GL_FRAMEBUFFER_SRGB); 

	// wireframe rendering mode for debugging.
	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// init GLResources
	GLResources.TextureIndex = 0;
	GLResources.ShaderProgramIndex = 0;
	GLResources.RenderTargetIndex = 0;
	GLResources.State.BoundRenderTarget = 0;

	// init batches
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
	// create shader program
	read_file_result VertShdr, FragShdr;

	VertShdr = Platform->ReadFile("shader/texture_quad.vs");
	FragShdr = Platform->ReadFile("shader/texture_quad.fs");

	TextureQuadBatch.PROGIndex = GLCreateProgram(&VertShdr, &FragShdr);

	Platform->FreeFileMemory(&VertShdr);
	Platform->FreeFileMemory(&FragShdr);

	// allocate buffer
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
	// create shader
	read_file_result VertShdr, FragShdr;

	VertShdr = Platform->ReadFile("shader/color_quad.vs");
	FragShdr = Platform->ReadFile("shader/color_quad.fs");

	ColorQuadBatch.PROGIndex = GLCreateProgram(&VertShdr, &FragShdr);

	Platform->FreeFileMemory(&VertShdr);
	Platform->FreeFileMemory(&FragShdr);

	// allocate buffer
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
	// create shader
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
	// create shader
	read_file_result VertShdr, FragShdr;

	VertShdr = Platform->ReadFile("shader/line.vs");
	FragShdr = Platform->ReadFile("shader/line.fs");

	LineBatch.PROGIndex = GLCreateProgram(&VertShdr, &FragShdr);

	Platform->FreeFileMemory(&VertShdr);
	Platform->FreeFileMemory(&FragShdr);

	// allocate buffer
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

u32 GLUploadTexture(texture *Texture, Renderer::TextureTarget Target, Renderer::TextureParam Filter,
					Renderer::TextureParam Wrap)
{
	Assert(GLResources.TextureIndex < ArrayCount(GLResources.Texture));

	GLenum GLTarget = 0, GLFilter = 0, GLWrap = 0;

	if(Target == Renderer::TEXTURE_2D)
	{
		GLTarget = GL_TEXTURE_2D;
	}
	else
	{
		Assert(!"Invalid texture target.");
	}

	if(Filter == Renderer::NEAREST)
	{
		GLFilter = GL_NEAREST;
	}
	else if(Filter == Renderer::LINEAR)
	{
		GLFilter = GL_LINEAR;
	}
	else
	{
		Assert(!"Invalid texture filter parameter.");
	}

	if(Wrap == Renderer::REPEAT)
	{
		GLWrap = GL_REPEAT;
	}
	else if(Wrap == Renderer::CLAMP_TO_EDGE)
	{
		GLWrap = GL_CLAMP_TO_EDGE;
	}
	else
	{
		Assert(!"Invalid texture wrap parameter.");
	}

	// generate and bind Texture
	glGenTextures(1, &GLResources.Texture[GLResources.TextureIndex].TEXHandle);
	glBindTexture(GLTarget, GLResources.Texture[GLResources.TextureIndex].TEXHandle);

	// orient the texture
	if(!Texture->FlippedVertically)
	{
		Texture->FlipVertically();
	}

	// upload the texture to the GPU
	glTexImage2D(GLTarget, 0, GL_SRGB_ALPHA, Texture->Width, Texture->Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, Texture->Content);
	
	// set texture parameters
	glTexParameteri(GLTarget, GL_TEXTURE_WRAP_S, GLWrap);
	glTexParameteri(GLTarget, GL_TEXTURE_WRAP_T, GLWrap);
	glTexParameteri(GLTarget, GL_TEXTURE_MIN_FILTER, GLFilter);
	glTexParameteri(GLTarget, GL_TEXTURE_MAG_FILTER, GLFilter);

	// unbind texture
	glBindTexture(GLTarget, 0);

	return GLResources.TextureIndex++;
}

u32 GLDeleteTexture()
{
	return 0;
}

u32 GLCreateRenderTarget(u32 Width, u32 Height, Renderer::TextureParam Filter)
{
	Assert(GLResources.RenderTargetIndex < ArrayCount(GLResources.RenderTarget));

	u32 TargetIndex = ++GLResources.RenderTargetIndex;
	
	GLenum GLFilter = 0;
	if(Filter == Renderer::NEAREST)
	{
		GLFilter = GL_NEAREST;
	}
	else if(Filter == Renderer::LINEAR)
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

	// generate & bind FBO
	glGenFramebuffers(1, &FBOHandle);
	glBindFramebuffer(GL_FRAMEBUFFER, FBOHandle);

	// generate and bind ColorTexture
	glGenTextures(1, &ColorTEXHandle);
	glBindTexture(GL_TEXTURE_2D, ColorTEXHandle);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, Width, Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GLFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GLFilter);

	// attach ColorTexture to FBO
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ColorTEXHandle, 0);

	// unbind ColorTexture 
	glBindTexture(GL_TEXTURE_2D, 0);

	// generate and bind DepthRBO
	glGenRenderbuffers(1, &DepthRBOHandle);
	glBindRenderbuffer(GL_RENDERBUFFER, DepthRBOHandle);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, Width, Height);

	// attach DepthRBO to FBO
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, DepthRBOHandle);
	
	// unbind DepthRBO
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	// check if framebuffer is complete
	Assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

	// clear framebuffer
	glClearColor(0.3f, 0.3f, 0.3f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// bind default framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return GLResources.RenderTargetIndex;
}

u32 GLDeleteRenderTarget()
{
	return 0;
}

void GLClear()
{	
	// for(int Index = GLResources.RenderTargetIndex; )
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

	mat4 PosOffset = mat4::Translate(0.5f/Platform->Width, 0.5f/Platform->Height, 0);
	mat4 Proj = mat4::Orthographic(0.0f, (r32)Platform->Width, (r32)Platform->Height, 0.0f, -1.0f, 1.0f);
	mat4 FinalMatrix = Proj*PosOffset;

	glUniformMatrix4fv(LineBatch.projectionUniform, 1, GL_TRUE, FinalMatrix.Elements);
	
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

	mat4 PosOffset = mat4::Translate(0.5f/Platform->Width, 0.5f/Platform->Height, 0);
	mat4 Proj = mat4::Orthographic(0.0f, (r32)Platform->Width, (r32)Platform->Height, 0.0f, -1.0f, 1.0f);
	mat4 FinalMatrix = Proj*PosOffset;

	glUniformMatrix4fv(TextureQuadBatch.projectionUniform, 1, GL_TRUE, FinalMatrix.Elements);
	
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

	mat4 PosOffset = mat4::Translate(0.5f/Platform->Width, 0.5f/Platform->Height, 0);
	mat4 Proj = mat4::Orthographic(0.0f, (r32)Platform->Width, (r32)Platform->Height, 0.0f, -1.0f, 1.0f);
	mat4 FinalMatrix = Proj*PosOffset;
	glUniformMatrix4fv(TextBatch.projectionUniform, 1, GL_TRUE, FinalMatrix.Elements);
	
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

	mat4 PosOffset = mat4::Translate(0.5f/Platform->Width, 0.5f/Platform->Height, 0);
	mat4 Proj = mat4::Orthographic(0.0f, (r32)Platform->Width, (r32)Platform->Height, 0.0f, -1.0f, 1.0f);
	mat4 FinalMatrix = Proj*PosOffset;

	glUniformMatrix4fv(ColorQuadBatch.projectionUniform, 1, GL_TRUE, FinalMatrix.Elements);
	
	glDrawArrays(GL_TRIANGLES, 0, Count * 6);

	glBindVertexArray(0);
	glUseProgram(0);
}

void GLDrawTargetQuad(void *Data, u32 RenderTargetIndex)
{
	if(GLResources.State.BoundRenderTarget != 0)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	glUseProgram(GLResources.ShaderProgram[TextureQuadBatch.PROGIndex].PROGHandle);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, GLResources.RenderTarget[RenderTargetIndex].ColorTEXHandle);
	glUniform1i(TextureQuadBatch.map0Uniform, 0);

	glBindBuffer(GL_ARRAY_BUFFER, TextureQuadBatch.VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 54 * sizeof(GLfloat), Data);

	glBindVertexArray(TextureQuadBatch.VAO);

	mat4 PosOffset = mat4::Translate(0.5f/Platform->Width, 0.5f/Platform->Height, 0);
	mat4 Proj = mat4::Orthographic(0.0f, (r32)Platform->Width, (r32)Platform->Height, 0.0f, -1.0f, 1.0f);
	mat4 FinalMatrix = Proj*PosOffset;

	glUniformMatrix4fv(TextureQuadBatch.projectionUniform, 1, GL_TRUE, FinalMatrix.Elements);
	
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glBindVertexArray(0);
	glUseProgram(0);
}

void GLDrawDebugAxis()
{
	glUseProgram(0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	static r32 Dy = 0.0f, Dx = 0.0f; 
	mat4 LookAt = mat4::LookAt(vec3(Dx,Dy,3.0f), vec3(0,0,0), vec3(0,1,0));
	// Dy += 0.001f;
	// Dx += 0.001f;
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
}

