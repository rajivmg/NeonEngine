#ifndef DEBUGDRAW_H
#define DEBUGDRAW_H

#include <core/neon_renderer.h>
#include <core/neon_font.h>

struct debug_draw
{
    mat4 ScreenProjMatrix;
    mat4 ScreenViewMatrix;
    render_resource DbgRectShader;
    render_resource DbgRectVertexBuffer;
    std::vector<vert_P1C1> DbgRectVertices;
    render_cmd_list *DbgRectRender;

    font DbgFont;
    render_resource DbgTextShader;
    render_resource DbgTextVertexBuffer;
    std::vector<vert_P1C1UV1> DbgTextVertices;
    render_cmd_list *DbgTextRender;

    render_resource DbgLineShader;
    render_resource DbgLineVertexBuffer;
    std::vector<vert_P1C1> DbgLineVertices;
    render_cmd_list *DbgLineRender;

    debug_draw();
    ~debug_draw();
    void Rect(rect Dest, vec4 Color, r32 Layer);
    void Text(rect Dest, vec4 Color, r32 Layer, char const *Format, ...);
    void Line(vec3 FromP, vec3 ToP, vec4 Color);
    void Submit();
};
extern debug_draw *DebugDraw;
#endif