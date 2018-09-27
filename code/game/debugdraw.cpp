#include "debugdraw.h"

#include <stdarg.h>

debug_draw::debug_draw()
{
    ScreenProjMatrix = Orthographic(0.0f, 1280.0f, 720.0f, 0.0f, -10.0f, 1.0f);
    ScreenViewMatrix = LookAt(vec3(0.0f), vec3i(0, 0, -1), vec3i(0, 1, 0));

    DbgRectShader = rndr::MakeShaderProgram("shaders/debug_rect_vs.glsl", "shaders/debug_rect_ps.glsl");
    DbgRectVertexBuffer = rndr::MakeBuffer(resource_type::VERTEX_BUFFER, MEGABYTE(1), true);
    DbgRectRender = new render_cmd_list(MEGABYTE(1), DbgRectShader, &ScreenViewMatrix, &ScreenProjMatrix);

    InitFont(&DbgFont, "fonts/inconsolata_26.fnt");
    DbgTextShader = rndr::MakeShaderProgram("shaders/sprite_vs.glsl", "shaders/sprite_ps.glsl");
    DbgTextVertexBuffer = rndr::MakeBuffer(resource_type::VERTEX_BUFFER, MEGABYTE(1), true);
    DbgTextRender = new render_cmd_list(MEGABYTE(1), DbgTextShader, &ScreenViewMatrix, &ScreenProjMatrix);

    DbgLineShader = rndr::MakeShaderProgram("shaders/debug_line_vs.glsl", "shaders/debug_line_ps.glsl");
    DbgLineVertexBuffer = rndr::MakeBuffer(resource_type::VERTEX_BUFFER, MEGABYTE(5), true);
    DbgLineRender = new render_cmd_list(MEGABYTE(1), DbgLineShader, &ScreenViewMatrix, &ScreenProjMatrix);
}

debug_draw::~debug_draw()
{
    rndr::DeleteShaderProgram(DbgRectShader);
    rndr::DeleteBuffer(DbgRectVertexBuffer);
    rndr::DeleteShaderProgram(DbgTextShader);
    rndr::DeleteBuffer(DbgTextVertexBuffer);
    rndr::DeleteShaderProgram(DbgLineShader);
    rndr::DeleteBuffer(DbgLineVertexBuffer);
}

void debug_draw::Rect(rect Dest, vec4 Color, r32 Layer)
{
    PushDbgRect(&DebugDraw->DbgRectVertices, Dest, Color, Layer);
}

void debug_draw::Text(rect Dest, vec4 Color, r32 Layer, char const *Format, ...)
{
    va_list ArgList;
    va_start(ArgList, Format);
    PushTextV(&DebugDraw->DbgTextVertices, Dest, Color, Layer, &DbgFont, Format, ArgList);
    va_end(ArgList);
}

void debug_draw::Line(vec3 FromP, vec3 ToP, vec4 Color)
{
    PushDbgLine(&DebugDraw->DbgLineVertices, FromP, ToP, Color);
}

void debug_draw::Submit()
{
    if(!DbgRectVertices.empty())
    {
        rndr::BufferData(DbgRectVertexBuffer, 0, (u32)DbgRectVertices.size() * (u32)sizeof(vert_P1C1), &DbgRectVertices.front());

        cmd::draw *DrawDebugRectCmd = DbgRectRender->AddCommand<cmd::draw>(0);
        DrawDebugRectCmd->VertexBuffer = DbgRectVertexBuffer;
        DrawDebugRectCmd->VertexFormat = vert_format::P1C1;
        DrawDebugRectCmd->StartVertex = 0;
        DrawDebugRectCmd->VertexCount = (u32)DbgRectVertices.size();

        DbgRectVertices.clear();

        DbgRectRender->Sort();
        DbgRectRender->Submit();
        DbgRectRender->Flush();
    }
    if(!DbgTextVertices.empty())
    {
        rndr::BufferData(DbgTextVertexBuffer, 0, (u32)sizeof(vert_P1C1UV1) * (u32)DbgTextVertices.size(), &DbgTextVertices.front());

        cmd::draw *DebugTextCmd = DbgTextRender->AddCommand<cmd::draw>(0);
        DebugTextCmd->VertexBuffer = DbgTextVertexBuffer;
        DebugTextCmd->VertexFormat = vert_format::P1C1UV1;
        DebugTextCmd->StartVertex = 0;
        DebugTextCmd->VertexCount = (u32)DbgTextVertices.size();
        DebugTextCmd->Textures[0] = DbgFont.Texture;

        DbgTextVertices.clear();

        DbgTextRender->Sort();
        DbgTextRender->Submit();
        DbgTextRender->Flush();
    }
    if(!DbgLineVertices.empty())
    {
        rndr::BufferData(DbgLineVertexBuffer, 0, (u32)sizeof(vert_P1C1) * (u32)DbgLineVertices.size(), &DbgLineVertices.front());

        cmd::draw_debug_lines *DrawDebugLinesCmd = DbgLineRender->AddCommand<cmd::draw_debug_lines>(0);
        DrawDebugLinesCmd->VertexBuffer = DbgLineVertexBuffer;
        DrawDebugLinesCmd->VertexFormat = vert_format::P1C1;
        DrawDebugLinesCmd->StartVertex = 0;
        DrawDebugLinesCmd->VertexCount = (u32)DbgLineVertices.size();

        DbgLineVertices.clear();

        DbgLineRender->Sort();
        DbgLineRender->Submit();
        DbgLineRender->Flush();
    }
}