#include "neon_opengl.h"

#include "neon_renderer.h"
#include "neon_bitmap.h"
#include <dear-imgui/imgui.h>

#define NEON_GL_IMPLEMENTATION
#define NEON_DEBUG_GL
#include "neon_GL.h"

static render_state RenderState = {};

void ogl::InitState()
{
    InitGL();

    RenderState.TextureCurrent = 0;
    RenderState.ShaderProgramCurrent = 0;

    // Set the 4/3 viewport
    //u32 ViewportWidth, ViewportHeight;
    //s32 ViewportX, ViewportY;

    //ViewportWidth = (u32)(Platform.WindowWidth * (3.0f / 4.0f));
    //ViewportHeight = (u32)(Platform.WindowHeight * (4.0f / 3.0f));
    //ViewportX = (Platform.WindowWidth - ViewportWidth) / 2.0f;
    //glViewport(ViewportX, 0, ViewportWidth, Platform.WindowHeight);

    glViewport(0, 0, (u32)Platform.WindowWidth, (u32)Platform.WindowHeight);

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Set CCW vertex winding as triangle's front.
    glFrontFace(GL_CCW);

    // Enable back-face culling
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // Enable the alpha blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Src.rgb * Src_Alpha + Dest.rgb * (1 - Src.Alpha) aka post-multiplied alpha blending
    //glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA); // Src.rgb * One + Dest.rgb * (1 - Src.Alpha) aka pre-multiplied alpha blending

    // Enable SRGB framebuffer
    // This converts fragments shader linear output to gamma correct output i.e. apply pow(1/2.2) to the colors
    // @NOTE: fragment shader must always output in linear color space
    // TODO: Check if SRGB framebuffer is supported.
    //glEnable(GL_FRAMEBUFFER_SRGB);

    // Set clear color
    //glClearColor(0.07f, 0.07f, 0.08f, 1.0f);
    vec4 ClearColor = RGBAUnpackTo01(0xdbdbdbff);
    glClearColor(ClearColor.r, ClearColor.g, ClearColor.b, ClearColor.a);

    // Enable multisampling
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

    // Wireframe rendering mode for debugging.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void ogl::Clear()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void ogl::SetViewMatrix(mat4 Matrix)
{
    glUniformMatrix4fv(RenderState.ShaderPrograms[RenderState.ActiveShaderProgram].ViewMatrix, 1, GL_FALSE, Matrix.Elements);
}

void ogl::SetProjectionMatrix(mat4 Matrix)
{
    glUniformMatrix4fv(RenderState.ShaderPrograms[RenderState.ActiveShaderProgram].ProjMatrix, 1, GL_FALSE, Matrix.Elements);
}

render_resource ogl::MakeTexture(bitmap *Bitmap, tex_param Type, tex_param Filter, tex_param Wrap, bool HwGammaCorrection)
{
    ASSERT(RenderState.TextureCurrent < ARRAY_COUNT(RenderState.Textures));

    render_resource RenderResource;
    RenderResource.Type = resource_type::TEXTURE;
    RenderResource.ResourceHandle = RenderState.TextureCurrent++;

    // Generate and bind Texture
    glGenTextures(1, &RenderState.Textures[RenderResource.ResourceHandle]);
    // TODO: Add more texture type in future.
    ASSERT(Type == tex_param::TEX2D);
    glBindTexture(GL_TEXTURE_2D, RenderState.Textures[RenderResource.ResourceHandle]);

    // Orient the texture
    if(!Bitmap->FlippedAroundY)
        BitmapFlipAroundY(Bitmap);

    // Upload the texture to the GPU
    glTexImage2D(GL_TEXTURE_2D, 0, HwGammaCorrection ? GL_SRGB_ALPHA : GL_RGBA, Bitmap->Width, Bitmap->Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, Bitmap->Data);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, Wrap == tex_param::REPEAT ? GL_REPEAT : GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, Wrap == tex_param::REPEAT ? GL_REPEAT : GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, Filter == tex_param::NEAREST ? GL_NEAREST : GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, Filter == tex_param::NEAREST ? GL_NEAREST : GL_LINEAR);

    // Unbind texture
    glBindTexture(GL_TEXTURE_2D, 0);

    return RenderResource;
}

void ogl::DeleteTexture(render_resource Texture)
{
    glDeleteTextures(1, &RenderState.Textures[Texture.ResourceHandle]);
}

void *ogl::GetTextureID(render_resource Texture)
{
    return (void *)(iptr)RenderState.Textures[Texture.ResourceHandle];
}

render_resource ogl::MakeBuffer(resource_type Type, u32 Size, bool Dynamic)
{
    GLenum BufferEnum;
    switch(Type)
    {
        case resource_type::VERTEX_BUFFER:
        {
            BufferEnum = GL_ARRAY_BUFFER;
        } break;

        case resource_type::INDEX_BUFFER:
        {
            BufferEnum = GL_ELEMENT_ARRAY_BUFFER;
        } break;

        case resource_type::CONSTANT_BUFFER:
        {
            BufferEnum = GL_UNIFORM_BUFFER;
        } break;

        INVALID_DEFAULT_CASE;
    }

    ASSERT(Size > 0);

    render_resource RenderResource;
    RenderResource.Type = Type;
    RenderResource.ResourceHandle = RenderState.BufferObjectCount++;
    RenderState.BufferObjects[RenderResource.ResourceHandle].Capacity = Size;
    RenderState.BufferObjects[RenderResource.ResourceHandle].IsDynamic = Dynamic;

    glGenBuffers(1, &RenderState.BufferObjects[RenderResource.ResourceHandle].Buffer);
    glBindBuffer(BufferEnum, RenderState.BufferObjects[RenderResource.ResourceHandle].Buffer);
    glBufferData(BufferEnum, Size, nullptr, Dynamic ? GL_STREAM_DRAW : GL_STATIC_DRAW);
    glBindBuffer(BufferEnum, 0);

    return RenderResource;
}

void ogl::BufferData(render_resource Buffer, u32 Offset, u32 Size, void const *Data)
{
    GLenum BufferEnum;
    switch(Buffer.Type)
    {
        case resource_type::VERTEX_BUFFER:
        {
            BufferEnum = GL_ARRAY_BUFFER;
        } break;

        case resource_type::INDEX_BUFFER:
        {
            BufferEnum = GL_ELEMENT_ARRAY_BUFFER;
        } break;

        case resource_type::CONSTANT_BUFFER:
        {
            BufferEnum = GL_UNIFORM_BUFFER;
        } break;

        INVALID_DEFAULT_CASE;
    }

    ASSERT(Size > 0 && Size <= RenderState.BufferObjects[Buffer.ResourceHandle].Capacity);

    glBindBuffer(BufferEnum, RenderState.BufferObjects[Buffer.ResourceHandle].Buffer);
    glBufferSubData(BufferEnum, Offset, Size, Data);
    glBindBuffer(BufferEnum, 0);
}

void ogl::DeleteBuffer(render_resource Buffer)
{
    glDeleteBuffers(1, &RenderState.BufferObjects[Buffer.ResourceHandle].Buffer);
    RenderState.BufferObjects[Buffer.ResourceHandle].Capacity = 0;
    RenderState.BufferObjects[Buffer.ResourceHandle].IsDynamic = false;
}

void ogl::BindBuffer(render_resource Buffer, u32 Index)
{
    GLenum BufferEnum = GL_UNIFORM_BUFFER;
    glBindBufferBase(BufferEnum, Index, RenderState.BufferObjects[Buffer.ResourceHandle].Buffer);
}

render_resource ogl::MakeShaderProgram(char const *VertShaderSrc, char const *FragShaderSrc)
{
    render_resource RenderResource;
    RenderResource.Type = resource_type::SHADER_PROGRAM;
    RenderResource.ResourceHandle = RenderState.ShaderProgramCurrent++;

    file_content VsFile = Platform.ReadFile(VertShaderSrc);
    file_content FsFile = Platform.ReadFile(FragShaderSrc);
    ASSERT(VsFile.NoError);
    ASSERT(FsFile.NoError);

    GLuint Vs = glCreateShader(GL_VERTEX_SHADER);
    GLuint Fs = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(Vs, 1, (GLchar * const *)(&VsFile.Content), (const GLint *)(&VsFile.Size));
    glShaderSource(Fs, 1, (GLchar * const *)(&FsFile.Content), (const GLint *)(&FsFile.Size));

    Platform.FreeFileContent(&VsFile);
    Platform.FreeFileContent(&FsFile);

    glCompileShader(Vs);
    glCompileShader(Fs);

    shader_program *ShaderProgram = &RenderState.ShaderPrograms[RenderResource.ResourceHandle];

    ShaderProgram->Program = glCreateProgram();
    glAttachShader(ShaderProgram->Program, Vs);
    glAttachShader(ShaderProgram->Program, Fs);
    glLinkProgram(ShaderProgram->Program);

    glValidateProgram(ShaderProgram->Program);
    GLint Validated;
    glGetProgramiv(ShaderProgram->Program, GL_VALIDATE_STATUS, &Validated);

    if(!Validated)
    {
        char VsErrors[8192];
        char FsErrors[8192];
        char ProgramErrors[8192];
        glGetShaderInfoLog(Vs, sizeof(VsErrors), 0, VsErrors);
        glGetShaderInfoLog(Fs, sizeof(FsErrors), 0, FsErrors);
        glGetProgramInfoLog(ShaderProgram->Program, sizeof(ProgramErrors), 0, ProgramErrors);

        ASSERT(!"Shader compilation and/or linking failed");
    }

    glDetachShader(ShaderProgram->Program, Vs);
    glDetachShader(ShaderProgram->Program, Fs);
    glDeleteShader(Vs);
    glDeleteShader(Fs);

    // Store sampler2D information
    glUseProgram(ShaderProgram->Program); // Bind shader program to use glUniform

    char *SamplerNames[] =
    {
        "Sampler0",
        "Sampler1",
        "Sampler2",
        "Sampler3",
        "Sampler4",
        "Sampler5",
        "Sampler6",
        "Sampler7"
    };
    for(int I = 0; I < ARRAY_COUNT(SamplerNames); ++I)
    {
        char *SamplerName = SamplerNames[I];
        ShaderProgram->Sampler[I] = glGetUniformLocation(ShaderProgram->Program, SamplerName);
        if(ShaderProgram->Sampler[I] != -1)
        {
            glUniform1i(ShaderProgram->Sampler[I], I);
            ++ShaderProgram->SamplerCount;
        }
    }

    // Store projection matrix location
    ShaderProgram->ProjMatrix = glGetUniformLocation(ShaderProgram->Program, "Projection");
    ShaderProgram->ViewMatrix = glGetUniformLocation(ShaderProgram->Program, "View");

    return RenderResource;
}

void ogl::DeleteShaderProgram(render_resource ShaderProgram)
{
    ASSERT(ShaderProgram.Type == resource_type::SHADER_PROGRAM);
    glDeleteProgram(RenderState.ShaderPrograms[ShaderProgram.ResourceHandle].Program);
    RenderState.ShaderPrograms[ShaderProgram.ResourceHandle].SamplerCount = 0;
}

void ogl::UseShaderProgram(render_resource ShaderProgram)
{
    glUseProgram(RenderState.ShaderPrograms[ShaderProgram.ResourceHandle].Program);
    RenderState.ActiveShaderProgram = ShaderProgram.ResourceHandle;
}

void ogl::UpdateUniform(char const *UniformName, r32 Value)
{
    GLint Loc = glGetUniformLocation(RenderState.ShaderPrograms[RenderState.ActiveShaderProgram].Program, UniformName);
    glUniform1f(Loc, Value);
}

void ogl::Draw(cmd::draw *Cmd)
{
    glBindBuffer(GL_ARRAY_BUFFER, RenderState.BufferObjects[Cmd->VertexBuffer.ResourceHandle].Buffer);

    if(Cmd->VertexFormat == vert_format::P1C1UV1)
    {
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vert_P1C1UV1), (void *)OFFSET_OF(vert_P1C1UV1, Position));
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vert_P1C1UV1), (void *)OFFSET_OF(vert_P1C1UV1, UV));
        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(vert_P1C1UV1), (void *)OFFSET_OF(vert_P1C1UV1, Color));

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);

        u32 TextureSlot = 0;
        while(TextureSlot < ARRAY_COUNT(shader_program::Sampler))
        {
            if(Cmd->Textures[TextureSlot].Type != resource_type::NOT_INITIALIZED)
            {
                glActiveTexture(GL_TEXTURE0 + TextureSlot);
                glBindTexture(GL_TEXTURE_2D, RenderState.Textures[Cmd->Textures[TextureSlot].ResourceHandle]);
            }
            ++TextureSlot;
        }

        glDrawArrays(GL_TRIANGLES, Cmd->StartVertex, Cmd->VertexCount);

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);
    }
}

void ogl::DrawIndexed(cmd::draw_indexed *Cmd)
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, RenderState.BufferObjects[Cmd->IndexBuffer.ResourceHandle].Buffer);
    glBindBuffer(GL_ARRAY_BUFFER, RenderState.BufferObjects[Cmd->VertexBuffer.ResourceHandle].Buffer);

    if(Cmd->VertexFormat == vert_format::P1C1UV1)
    {
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vert_P1C1UV1), (void *)OFFSET_OF(vert_P1C1UV1, Position));
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vert_P1C1UV1), (void *)OFFSET_OF(vert_P1C1UV1, UV));
        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(vert_P1C1UV1), (void *)OFFSET_OF(vert_P1C1UV1, Color));

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);

        u32 TextureSlot = 0;
        while(TextureSlot < ARRAY_COUNT(shader_program::Sampler))
        {
            if(Cmd->Textures[TextureSlot].Type != resource_type::NOT_INITIALIZED)
            {
                glActiveTexture(GL_TEXTURE0 + TextureSlot);
                glBindTexture(GL_TEXTURE_2D, RenderState.Textures[Cmd->Textures[TextureSlot].ResourceHandle]);
            }
            ++TextureSlot;
        }

        glDrawElements(GL_TRIANGLES, Cmd->IndexCount, GL_UNSIGNED_SHORT, 0);

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);
    }
    else if(Cmd->VertexFormat == vert_format::P1N1UV1)
    {
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vert_P1N1UV1), (void *)OFFSET_OF(vert_P1C1UV1, Position));
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vert_P1N1UV1), (void *)OFFSET_OF(vert_P1N1UV1, Normal));
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vert_P1N1UV1), (void *)OFFSET_OF(vert_P1N1UV1, UV));

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);

        u32 TextureSlot = 0;
        while(TextureSlot < ARRAY_COUNT(shader_program::Sampler))
        {
            if(Cmd->Textures[TextureSlot].Type != resource_type::NOT_INITIALIZED)
            {
                glActiveTexture(GL_TEXTURE0 + TextureSlot);
                glBindTexture(GL_TEXTURE_2D, RenderState.Textures[Cmd->Textures[TextureSlot].ResourceHandle]);
            }
            ++TextureSlot;
        }

        glDrawElements(GL_TRIANGLES, Cmd->IndexCount, GL_UNSIGNED_SHORT, 0);

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);
    }
}

void ogl::DrawDebugLines(cmd::draw_debug_lines *Cmd)
{
    ASSERT(Cmd->VertexFormat == vert_format::P1C1);

    glBindBuffer(GL_ARRAY_BUFFER, RenderState.BufferObjects[Cmd->VertexBuffer.ResourceHandle].Buffer);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vert_P1C1), (void *)OFFSET_OF(vert_P1C1, Position));
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(vert_P1C1), (void *)OFFSET_OF(vert_P1C1, Color));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glDrawArrays(GL_LINES, Cmd->StartVertex, Cmd->VertexCount);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}

//-----------------------------------------------------------------------------
// ImGui Rendering
//-----------------------------------------------------------------------------

// ImGui Data
static GLuint       imgui_FontTexture = 0;
static int          imgui_ShaderHandle = 0, imgui_VertHandle = 0, imgui_FragHandle = 0;
static int          imgui_AttribLocationTex = 0, imgui_AttribLocationProjMtx = 0;
static int          imgui_AttribLocationPosition = 0, imgui_AttribLocationUV = 0, imgui_AttribLocationColor = 0;
static unsigned int imgui_VboHandle = 0, imgui_VaoHandle = 0, imgui_ElementsHandle = 0;

void ImGui_RenderDrawLists(ImDrawData* draw_data)
{
    // Avoid rendering when minimized, scale coordinates for retina displays (screen coordinates != framebuffer coordinates)
    ImGuiIO& io = ImGui::GetIO();
    int fb_width = (int)(io.DisplaySize.x * io.DisplayFramebufferScale.x);
    int fb_height = (int)(io.DisplaySize.y * io.DisplayFramebufferScale.y);
    if(fb_width == 0 || fb_height == 0)
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
        { 2.0f / io.DisplaySize.x, 0.0f,                   0.0f, 0.0f },
        { 0.0f,                  2.0f / -io.DisplaySize.y, 0.0f, 0.0f },
        { 0.0f,                  0.0f,                  -1.0f, 0.0f },
        {-1.0f,                  1.0f,                   0.0f, 1.0f },
    };
    glUseProgram(imgui_ShaderHandle);
    glUniform1i(imgui_AttribLocationTex, 0);
    glUniformMatrix4fv(imgui_AttribLocationProjMtx, 1, GL_FALSE, &ortho_projection[0][0]);
    glBindVertexArray(imgui_VaoHandle);
    glBindSampler(0, 0); // Rely on combined texture/sampler state.

    for(int n = 0; n < draw_data->CmdListsCount; n++)
    {
        const ImDrawList* cmd_list = draw_data->CmdLists[n];
        const ImDrawIdx* idx_buffer_offset = 0;

        glBindBuffer(GL_ARRAY_BUFFER, imgui_VboHandle);
        glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)cmd_list->VtxBuffer.Size * sizeof(ImDrawVert), (const GLvoid*)cmd_list->VtxBuffer.Data, GL_STREAM_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, imgui_ElementsHandle);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx), (const GLvoid*)cmd_list->IdxBuffer.Data, GL_STREAM_DRAW);

        for(int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
        {
            const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
            if(pcmd->UserCallback)
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
    if(last_enable_blend) glEnable(GL_BLEND); else glDisable(GL_BLEND);
    if(last_enable_cull_face) glEnable(GL_CULL_FACE); else glDisable(GL_CULL_FACE);
    if(last_enable_depth_test) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
    if(last_enable_scissor_test) glEnable(GL_SCISSOR_TEST); else glDisable(GL_SCISSOR_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, last_polygon_mode[0]);
    glViewport(last_viewport[0], last_viewport[1], (GLsizei)last_viewport[2], (GLsizei)last_viewport[3]);
    glScissor(last_scissor_box[0], last_scissor_box[1], (GLsizei)last_scissor_box[2], (GLsizei)last_scissor_box[3]);
}

static void ImGui_CreateFontsTexture()
{
    // TODO: If even wanted to reload game dll then pass font texture atlas from main app for performance
    // Build texture atlas
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->AddFontFromFileTTF("fonts/DroidSans.ttf", 16);

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

bool ImGui_CreateDeviceObjects()
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
        "   Frag_UV = UV;\n"
        "   Frag_Color = Color;\n"
        "   gl_Position = ProjMtx * vec4(Position.xy,0,1);\n"
        "}\n";
    
    const GLchar* fragment_shader =
            "#version 330\n"
            "uniform sampler2D Texture;\n"
            "in vec2 Frag_UV;\n"
            "in vec4 Frag_Color;\n"
            "out vec4 Out_Color;\n"
            "void main()\n"
            "{\n"
            "   Out_Color = Frag_Color * texture( Texture, Frag_UV.st);\n"
            "}\n";

 /* const GLchar* fragment_shader =
        "#version 330\n"
        "uniform sampler2D Texture;\n"
        "in vec2 Frag_UV;\n"
        "in vec4 Frag_Color;\n"
        "out vec4 Out_Color;\n"
        "void main()\n"
        "{\n"
        "   vec4 sRGB_Color = Frag_Color * texture( Texture, Frag_UV.st);\n" // Colors already in sRGB color-space.
        "   Out_Color = vec4(pow(sRGB_Color.rgb, vec3(2.2)), sRGB_Color.a);\n" // Apply gamma so that when GL apply sRGB transformation we get correct results.
        "}\n"; */

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

void ImGui_InvalidateDeviceObjects()
{
    if(imgui_VaoHandle) glDeleteVertexArrays(1, &imgui_VaoHandle);
    if(imgui_VboHandle) glDeleteBuffers(1, &imgui_VboHandle);
    if(imgui_ElementsHandle) glDeleteBuffers(1, &imgui_ElementsHandle);
    imgui_VaoHandle = imgui_VboHandle = imgui_ElementsHandle = 0;

    if(imgui_ShaderHandle && imgui_VertHandle) glDetachShader(imgui_ShaderHandle, imgui_VertHandle);
    if(imgui_VertHandle) glDeleteShader(imgui_VertHandle);
    imgui_VertHandle = 0;

    if(imgui_ShaderHandle && imgui_FragHandle) glDetachShader(imgui_ShaderHandle, imgui_FragHandle);
    if(imgui_FragHandle) glDeleteShader(imgui_FragHandle);
    imgui_FragHandle = 0;

    if(imgui_ShaderHandle) glDeleteProgram(imgui_ShaderHandle);
    imgui_ShaderHandle = 0;

    if(imgui_FontTexture)
    {
        glDeleteTextures(1, &imgui_FontTexture);
        ImGui::GetIO().Fonts->TexID = 0;
        imgui_FontTexture = 0;
    }
}