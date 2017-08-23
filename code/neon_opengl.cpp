#include "neon_opengl.h"

static gl_batch TextureQuadBatch;
static gl_batch ColorQuadBatch;
static gl_batch TextBatch;

static gl_resources GLResources;

void GLInitRenderer()
{
	// Set clear color
	glClearColor(0.08f, 0.08f, 0.61f, 1.0); // gamma-ed

	// set the viewport
	glViewport(0, 0, Platform->Width, Platform->Height);
	
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
	// glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

	// init GLResources
	GLResources.TEXIndex = 0;

	// init batches
	GLInitTextureQuadBatch();
	GLInitColorQuadBatch();
	GLInitTextBatch();
}

static
void GLCreateProgram(GLuint *Program, read_file_result *VsFile, read_file_result *FsFile)
{
	GLuint Vs = glCreateShader(GL_VERTEX_SHADER);
	GLuint Fs = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(Vs, 1, (GLchar * const *)(&VsFile->Content), (const GLint *)(&VsFile->ContentSize));
	glShaderSource(Fs, 1, (GLchar * const *)(&FsFile->Content), (const GLint *)(&FsFile->ContentSize));

	glCompileShader(Vs);
	glCompileShader(Fs);

	*Program = glCreateProgram();
	glAttachShader(*Program, Vs);
	glAttachShader(*Program, Fs);

	glLinkProgram(*Program);
		
	glValidateProgram(*Program);
	GLint Validated;
	glGetProgramiv(*Program, GL_VALIDATE_STATUS, &Validated);
	
	if(!Validated)
	{
		char VsErrors[8192];
		char FsErrors[8192];
		char ProgramErrors[8192];
		glGetShaderInfoLog(Vs, sizeof(VsErrors), 0, VsErrors);
		glGetShaderInfoLog(Fs, sizeof(FsErrors), 0, FsErrors);
		glGetProgramInfoLog(*Program, sizeof(ProgramErrors), 0, ProgramErrors);

		Assert(!"Shader compilation and/or linking failed");	
	}	

	glDetachShader(*Program, Vs);
	glDetachShader(*Program, Fs);

	glDeleteShader(Vs);
	glDeleteShader(Fs);
}

static
void GLInitTextureQuadBatch()
{
	// create shader program
	read_file_result VertShdr, FragShdr;

	VertShdr = Platform->ReadFile("shader/texture_quad.vs");
	FragShdr = Platform->ReadFile("shader/texture_quad.fs");

	GLCreateProgram(&TextureQuadBatch.PROG, &VertShdr, &FragShdr);

	Platform->FreeFileMemory(&VertShdr);
	Platform->FreeFileMemory(&FragShdr);

	// allocate buffer
	GLuint PosLoc, ColorLoc, TexCoordLoc, map0Loc, ProjMatLoc;

	glGenBuffers(1, &TextureQuadBatch.VBO);
	glBindBuffer(GL_ARRAY_BUFFER, TextureQuadBatch.VBO);

	glBufferData(GL_ARRAY_BUFFER, MEGABYTE(8), 0, GL_DYNAMIC_DRAW);

	glGenVertexArrays(1, &TextureQuadBatch.VAO);
	glBindVertexArray(TextureQuadBatch.VAO);

	PosLoc = glGetAttribLocation(TextureQuadBatch.PROG, "vs_pos");
	Assert(PosLoc != -1);
	ColorLoc = glGetAttribLocation(TextureQuadBatch.PROG, "vs_color");
	Assert(ColorLoc != -1);
	TexCoordLoc = glGetAttribLocation(TextureQuadBatch.PROG, "vs_texcoord");
	Assert(TexCoordLoc != -1);

	map0Loc = glGetUniformLocation(TextureQuadBatch.PROG, "map0");
	Assert(map0Loc != -1);
	TextureQuadBatch.map0Uniform = map0Loc;

	ProjMatLoc = glGetUniformLocation(TextureQuadBatch.PROG, "projection");
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

	glGenTextures(1, &TextureQuadBatch.TEX);
}

static
void GLInitColorQuadBatch()
{
	// create shader
	read_file_result VertShdr, FragShdr;

	VertShdr = Platform->ReadFile("shader/color_quad.vs");
	FragShdr = Platform->ReadFile("shader/color_quad.fs");

	GLCreateProgram(&ColorQuadBatch.PROG, &VertShdr, &FragShdr);

	Platform->FreeFileMemory(&VertShdr);
	Platform->FreeFileMemory(&FragShdr);

	// allocate buffer
	GLuint PosLoc, ColorLoc, ProjMatLoc;

	glGenBuffers(1, &ColorQuadBatch.VBO);
	glBindBuffer(GL_ARRAY_BUFFER, ColorQuadBatch.VBO);

	glBufferData(GL_ARRAY_BUFFER, MEGABYTE(8), 0, GL_DYNAMIC_DRAW);

	glGenVertexArrays(1, &ColorQuadBatch.VAO);
	glBindVertexArray(ColorQuadBatch.VAO);

	PosLoc = glGetAttribLocation(ColorQuadBatch.PROG, "vs_pos");
	Assert(PosLoc != -1);
	ColorLoc = glGetAttribLocation(ColorQuadBatch.PROG, "vs_color");
	Assert(ColorLoc != -1);

	ProjMatLoc = glGetUniformLocation(ColorQuadBatch.PROG, "projection");
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

	GLCreateProgram(&TextBatch.PROG, &VertShdr, &FragShdr);

	Platform->FreeFileMemory(&VertShdr);
	Platform->FreeFileMemory(&FragShdr);

	// Create vertex buffers
	GLuint PosLoc, ColorLoc, TexCoordLoc, map0Loc, ProjMatLoc;

	glGenBuffers(1, &TextBatch.VBO);
	glBindBuffer(GL_ARRAY_BUFFER, TextBatch.VBO);

	glBufferData(GL_ARRAY_BUFFER, MEGABYTE(8), 0, GL_DYNAMIC_DRAW);

	glGenVertexArrays(1, &TextBatch.VAO);
	glBindVertexArray(TextBatch.VAO);

	PosLoc = glGetAttribLocation(TextBatch.PROG, "vs_pos");
	Assert(PosLoc != -1);
	ColorLoc = glGetAttribLocation(TextBatch.PROG, "vs_color");
	Assert(ColorLoc != -1);
	TexCoordLoc = glGetAttribLocation(TextBatch.PROG, "vs_texcoord");
	Assert(TexCoordLoc != -1);

	map0Loc = glGetUniformLocation(TextBatch.PROG, "map0");
	Assert(map0Loc != -1);
	TextBatch.map0Uniform = map0Loc;

	ProjMatLoc = glGetUniformLocation(TextBatch.PROG, "projection");
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
	
	glGenTextures(1, &TextBatch.TEX);
}

u32 GLUploadTexture(texture *Texture)
{
	Assert(GLResources.TEXIndex < ArrayCount(GLResources.TEX));

	glGenTextures(1, &GLResources.TEX[GLResources.TEXIndex]);
	glBindTexture(GL_TEXTURE_2D, GLResources.TEX[GLResources.TEXIndex]);


	if(!Texture->FlippedVertically)
	{
		Texture->FlipVertically();
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, Texture->Width, Texture->Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, Texture->Content);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	return GLResources.TEXIndex++;
}

u32  GLUploadMesh(mesh *Mesh)
{
	return 0;
}

void GLClear()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GLDrawTextureQuads(void *Data, u32 Count, u32 TextureIndex)
{
	glUseProgram(TextureQuadBatch.PROG);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, GLResources.TEX[TextureIndex]);
	glUniform1i(TextureQuadBatch.map0Uniform, 0);

	glBindBuffer(GL_ARRAY_BUFFER, TextureQuadBatch.VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 54 * Count * sizeof(GLfloat), Data);

	glBindVertexArray(TextureQuadBatch.VAO);

	mat4 Proj = mat4::Orthographic(0.0f, (r32)Platform->Width, (r32)Platform->Height, 0.0f, -1.0f, 1.0f);
	// mat4 Proj = mat4::Perspective(110, 16/9, 0.1f, 100.0f);

	glUniformMatrix4fv(TextureQuadBatch.projectionUniform, 1, GL_TRUE, Proj.Elements);
	
	glDrawArrays(GL_TRIANGLES, 0, Count * 6);
}

void GLDrawText(void *Data, u32 CharCount, u32 TextureIndex)
{
	glUseProgram(TextBatch.PROG);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, GLResources.TEX[TextureIndex]);
	glUniform1i(TextBatch.map0Uniform, 0);

	glBindBuffer(GL_ARRAY_BUFFER, TextBatch.VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 54 * CharCount * sizeof(GLfloat), Data);

	glBindVertexArray(TextBatch.VAO);

	mat4 Proj = mat4::Orthographic(0.0f, (r32)Platform->Width, (r32)Platform->Height, 0.0f, -1.0f, 1.0f);

	glUniformMatrix4fv(TextBatch.projectionUniform, 1, GL_TRUE, Proj.Elements);
	
	glDrawArrays(GL_TRIANGLES, 0, CharCount * 6);
}


void GLDrawColorQuads(void *Data, u32 Count)
{
	glUseProgram(ColorQuadBatch.PROG);

	glBindBuffer(GL_ARRAY_BUFFER, ColorQuadBatch.VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 42 * Count * sizeof(GLfloat), Data);

	glBindVertexArray(ColorQuadBatch.VAO);

	mat4 Proj = mat4::Orthographic(0.0f, (r32)Platform->Width, (r32)Platform->Height, 0.0f, -1.0f, 1.0f);

	glUniformMatrix4fv(ColorQuadBatch.projectionUniform, 1, GL_TRUE, Proj.Elements);
	
	glDrawArrays(GL_TRIANGLES, 0, Count * 6);
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

