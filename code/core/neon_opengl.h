#ifndef NEON_OPENGL_H
#define NEON_OPENGL_H

#include "neon_GL.h"

#include "neon_platform.h"
#include "neon_math.h"

class texture;
struct bitmap;
struct render_resource;
enum class resource_type : u32;

namespace cmd
{
    struct draw;
    struct draw_indexed;
    struct draw_debug_lines;
}

enum class tex_param;

struct buffer_object
{
    GLuint  Buffer;
    u32     Capacity;
    bool    IsDynamic; // TODO: Remove this??
};

struct shader_program
{
    GLuint  Program;
    u32     SamplerCount;
    GLint   Sampler[8];
    GLint   ProjMatrix;
    GLint   ViewMatrix;
};

// TODO: Reuse same slots after the buffer or texture is deleted.
struct render_state
{
    GLuint          Textures[1024];
    u32             TextureCurrent;

    shader_program  ShaderPrograms[1024];
    u32             ShaderProgramCurrent;

    buffer_object   BufferObjects[2048];
    u32             BufferObjectCount;

    u32             ActiveShaderProgram;
};

namespace ogl
{
    void            InitState();
    void            Clear();
    void            SetViewMatrix(mat4 Matrix);
    void            SetProjectionMatrix(mat4 Matrix);
    render_resource MakeTexture(bitmap *Bitmap, tex_param Type, tex_param Filter, tex_param Wrap, bool HwGammaCorrection);
    void            DeleteTexture(render_resource Texture);
    void*           GetTextureID(render_resource Texture);
    render_resource MakeBuffer(resource_type Type, u32 Size, bool Dynamic = false);
    void            BufferData(render_resource Buffer, u32 Offset, u32 Size, void const *Data);
    void            DeleteBuffer(render_resource Buffer);
    void            BindBuffer(render_resource Buffer, u32 Index);
    render_resource MakeShaderProgram(char const *VertShaderSrc, char const *FragShaderSrc);
    void            DeleteShaderProgram(render_resource ShaderProgram);
    void            UseShaderProgram(render_resource ShaderProgram);
    void            UpdateUniform(char const *UniformName, r32 Value);
    void            Draw(cmd::draw *Cmd);
    void            DrawIndexed(cmd::draw_indexed *Cmd);
    void            DrawDebugLines(cmd::draw_debug_lines *Cmd);
}
#endif