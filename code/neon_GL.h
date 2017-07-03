#ifndef NEON_GL_H
#define NEON_GL_H

#ifdef _MSC_VER
#include "Windows.h"
#define GLAPI WINAPI
#endif //_MSC_VER

#define GL_TEXTURE0                       0x84C0
#define GL_CLAMP_TO_EDGE                  0x812F
#define GL_FRAGMENT_SHADER                0x8B30
#define GL_VERTEX_SHADER                  0x8B31
#define GL_VALIDATE_STATUS                0x8B83
#define GL_ARRAY_BUFFER                   0x8892
#define GL_STREAM_DRAW                    0x88E0
#define GL_STATIC_DRAW                    0x88E4
#define GL_DYNAMIC_DRAW                   0x88E8

#define GL_DEBUG_OUTPUT_SYNCHRONOUS       0x8242

typedef char GLchar;
typedef ptrdiff_t GLintptr;
typedef ptrdiff_t GLsizeiptr;

#include <GL/gl.h>

#define GLPROCLIST \
GLPROC(GLuint, 	CreateShader, GLenum type)\
GLPROC(void, 	ShaderSource, GLuint shader, GLsizei count, const GLchar *const*string, const GLint *length)\
GLPROC(void, 	CompileShader, GLuint shader)\
GLPROC(void, 	DeleteShader, GLuint shader)\
GLPROC(void, 	DetachShader, GLuint program, GLuint shader)\
GLPROC(GLuint, 	CreateProgram, void)\
GLPROC(void,	AttachShader, GLuint program, GLuint shader)\
GLPROC(void,	LinkProgram, GLuint program)\
GLPROC(void,	UseProgram, GLuint program)\
GLPROC(void,	DeleteProgram, GLuint program)\
GLPROC(void,	ValidateProgram, GLuint program)\
GLPROC(void,	GetProgramiv, GLuint program, GLenum pname, GLint *params)\
GLPROC(void,	GetShaderInfoLog, GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog)\
GLPROC(void,	GetProgramInfoLog, GLuint program, GLsizei bufSize, GLsizei *length, GLchar *infoLog)\
GLPROC(void,	GenBuffers, GLsizei n, GLuint *buffers)\
GLPROC(void,	BindBuffer, GLenum target, GLuint buffer)\
GLPROC(void,	BufferData, GLenum target, GLsizeiptr size, const void *data, GLenum usage)\
GLPROC(void,	BufferSubData, GLenum target, GLintptr offset, GLsizeiptr size, const void *data)\
GLPROC(void,	GenVertexArrays, GLsizei n, GLuint *arrays)\
GLPROC(void,	BindVertexArray, GLuint array)\
GLPROC(GLint,	GetAttribLocation, GLuint program, const GLchar *name)\
GLPROC(void,	VertexAttribPointer, GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer)\
GLPROC(void,	EnableVertexAttribArray, GLuint index)\
GLPROC(GLint,	GetUniformLocation, GLuint program, const GLchar *name)\
GLPROC(void,	Uniform1i, GLint location, GLint v0)\
GLPROC(void, 	UniformMatrix4fv, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)\
GLPROC(void,	ActiveTexture, GLenum texture)

#ifdef NEON_DEBUG_GL
	#define GL_Assert(Exp) if(!(Exp)) {*(volatile int *)0 = 0;}
	#define GL_DEBUG_PROC(Name) void (GLAPI  Name)(GLenum source,GLenum type,GLuint id,GLenum severity,GLsizei length,const GLchar *message,const void *userParam)
	typedef GL_DEBUG_PROC(*GLDEBUGPROC);
	#define GLDEBUGPROCLIST \
	GLPROC(void, 	DebugMessageCallback, GLDEBUGPROC callback, void * userParam)
#else
	#define GLDEBUGPROCLIST
#endif

#define GLPROC(Ret, Name, ...) typedef Ret GLAPI gl_type_##Name (__VA_ARGS__); extern gl_type_##Name *gl##Name;
GLPROCLIST
GLDEBUGPROCLIST
#undef GLPROC

// typedef void 	GLAPI glUseProgram (GLuint program);

// extern gl_type_UseProgram *glUseProgram;

#ifdef NEON_DEBUG_GL
GL_DEBUG_PROC(OpenGLDebugCallback)
{
	char *Message = (char *)message;
	GL_Assert(!"OpenGL Debug Error");
}

void EnableGLDebug()
{
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(&OpenGLDebugCallback, 0);
}

void DisableGLDebug()
{
	glDisable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
}
#endif //NEON_DEBUG_GL



#ifdef NEON_INIT_GL 
#define GLPROC(Ret, Name, ...) gl_type_##Name *gl##Name;
GLPROCLIST
GLDEBUGPROCLIST
#undef GLPROC
bool InitGL()
{

#ifdef _MSC_VER

#define GLPROC(Ret, Name, ...) gl##Name = (gl_type_##Name *)wglGetProcAddress("gl"#Name); \
	if(gl##Name == 0) \
	{ \
		OutputDebugStringA("NEON_GL: gl" #Name "couldn't be loaded!\n"); \
	}
GLPROCLIST
GLDEBUGPROCLIST
#undef GLPROC

// glUseProgram = (gl_type_UseProgram *)wglGetProcAddress("glUseProgram");

#endif //_MSC_VER

#ifdef NEON_DEBUG_GL
EnableGLDebug();
#endif //NEON_DEBUG_GL

return true;
}
#endif //NEON_INIT_GL



#endif //NEON_GL_H