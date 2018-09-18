#include "neon_renderer.h"

#include "neon_font.h"
#include "neon_opengl.h"

dispatch_fn *cmd::draw::DISPATCH_FUNCTION = &rndr::Draw;
dispatch_fn *cmd::draw_indexed::DISPATCH_FUNCTION = &rndr::DrawIndexed;
dispatch_fn *cmd::copy_const_buffer::DISPATCH_FUNCTION = &rndr::CopyConstBuffer;
dispatch_fn *cmd::draw_debug_lines::DISPATCH_FUNCTION = &rndr::DrawDebugLines;

//-----------------------------------------------------------------------------
// Renderer Wrapper
//-----------------------------------------------------------------------------

void rndr::Init()
{
    // Initialise opengl renderer.
    ogl::InitState();
}

void rndr::Clear()
{
    ogl::Clear();
}

void rndr::SetViewMatrix(mat4 Matrix)
{
    ogl::SetViewMatrix(Matrix);
}

void rndr::SetProjectionMatrix(mat4 Matrix)
{
    ogl::SetProjectionMatrix(Matrix);
}

render_resource rndr::MakeTexture(bitmap *Bitmap, tex_param Type, tex_param Filter, tex_param Wrap, bool HwGammaCorrection)
{
    return ogl::MakeTexture(Bitmap, Type, Filter, Wrap, HwGammaCorrection);
}

void rndr::DeleteTexture(render_resource Texture)
{
    ogl::DeleteTexture(Texture);
}

void* rndr::GetTextureID(render_resource Texture)
{
    return ogl::GetTextureID(Texture);
}

render_resource rndr::MakeBuffer(resource_type Type, u32 Size, bool Dynamic)
{
    return ogl::MakeBuffer(Type, Size, Dynamic);
}

void rndr::BufferData(render_resource Buffer, u32 Offset, u32 Size, void const *Data)
{
    ogl::BufferData(Buffer, Offset, Size, Data);
}

void rndr::DeleteBuffer(render_resource Buffer)
{
    ogl::DeleteBuffer(Buffer);
}

void rndr::BindBuffer(render_resource Buffer, u32 Index)
{
    ogl::BindBuffer(Buffer, Index);
}

render_resource rndr::MakeShaderProgram(char const *VertShaderSrc, char const *FragShaderSrc)
{
    return ogl::MakeShaderProgram(VertShaderSrc, FragShaderSrc);
}

void rndr::DeleteShaderProgram(render_resource ShaderProgram)
{
    ogl::DeleteShaderProgram(ShaderProgram);
}

void rndr::UseShaderProgram(render_resource ShaderProgram)
{
    ogl::UseShaderProgram(ShaderProgram);
}

void rndr::Draw(void const *Data)
{
    cmd::draw *Cmd = (cmd::draw *)Data;
    ogl::Draw(Cmd);
}

void rndr::DrawIndexed(void const *Data)
{
    cmd::draw_indexed *Cmd = (cmd::draw_indexed *)Data;
    ogl::DrawIndexed(Cmd);
}

void rndr::CopyConstBuffer(void const * Data)
{
    cmd::copy_const_buffer *Cmd = (cmd::copy_const_buffer *)Data;
    ogl::BufferData(Cmd->ConstantBuffer, 0, Cmd->Size, Cmd->Data);
}

void rndr::DrawDebugLines(void const * Data)
{
    cmd::draw_debug_lines *Cmd = (cmd::draw_debug_lines *)Data;
    ogl::DrawDebugLines(Cmd);
}

//-----------------------------------------------------------------------------
// Render Commands
//-----------------------------------------------------------------------------

render_cmd_list::render_cmd_list(u32 _BufferSize, render_resource _ShaderProgram, mat4 *_ViewMatrix, mat4 *_ProjMatrix) :
    BufferSize(_BufferSize),
    ShaderProgram(_ShaderProgram),
    ViewMatrix(_ViewMatrix),
    ProjMatrix(_ProjMatrix),
    BaseOffset(0),
    Current(0)
{
    const u32 MaxPacketsInList = 2048;

    Buffer = MALLOC(BufferSize);
    memset(Buffer, 0, BufferSize);
    Keys = (u32 *)MALLOC(sizeof(u32) * MaxPacketsInList);
    Packets = (cmd_packet **)MALLOC(sizeof(cmd_packet *) * (MaxPacketsInList));
}

render_cmd_list::~render_cmd_list()
{
    SAFE_FREE(Buffer)
}

void *render_cmd_list::AllocateMemory(u32 MemorySize)
{
    ASSERT((BufferSize - BaseOffset) >= MemorySize);
    void *Mem = ((u8 *)Buffer + BaseOffset);
    BaseOffset += MemorySize;
    return Mem;
}

void render_cmd_list::Sort()
{
    // Sort using insertion sort.
    // TODO: Use Radix sort

    u32 i, j;
    for(i = 1; i < Current; ++i)
    {
        j = i;
        while((j > 0) && Keys[j - 1] > Keys[j])
        {
            cmd_packet *Temp = Packets[j];
            Packets[j] = Packets[j - 1];
            Packets[j - 1] = Temp;
            --j;
        }
    }
}

void render_cmd_list::Submit()
{
    rndr::UseShaderProgram(ShaderProgram);
    rndr::SetViewMatrix(*ViewMatrix);
    rndr::SetProjectionMatrix(*ProjMatrix);
    for(u32 I = 0; I < Current; ++I)
    {
        cmd_packet *Packet = Packets[I];

        for(;;)
        {
            dispatch_fn *DispatchFn = Packet->DispatchFn;
            void *Cmd = Packet->Cmd;
            DispatchFn(Cmd);

            Packet = Packet->NextCmdPacket;

            if(Packet == nullptr)
                break;
        }
    }
}

void render_cmd_list::Flush()
{
    memset(Buffer, 0, BaseOffset);
    Current = 0;
    BaseOffset = 0;
}

//-----------------------------------------------------------------------------
// Sprite and Text
//-----------------------------------------------------------------------------

void PushSprite(std::vector<vert_P1C1UV1> *Vertices, rect Dest, vec4 UV, vec4 Color, r32 Rotation, vec2 Origin, vec2 Scale, r32 Layer)
{
    /*
    0,1         1,1
    D-----------C
    |        /  |
    |      /    |
    |    /      |
    |  /        |
    A-----------B
    0,0         1,0
    (ACD)(ABC)
    */

    // TODO: Fast path when rotation is 0
    r32 SAng, CAng;
    SAng = sinf(Rotation);
    CAng = cosf(Rotation);

    // Translation factor to translate world origin aligned rectangle points to rectangle origin aligned points
    vec2 T = vec2(Dest.x + Origin.x, Dest.y + Origin.y);

    // Align rectangle origin with world origin
    vec2 P1, P2, P3, P4;

    P1 = vec2(-Origin.x, -Origin.y);
    P2 = vec2(Dest.width - Origin.x, -Origin.y);
    P3 = vec2(Dest.width - Origin.x, Dest.height - Origin.y);
    P4 = vec2(-Origin.x, Dest.height - Origin.y);

    // Scale world origin aligned rectangle points
    P1 = P1 * Scale;
    P2 = P2 * Scale;
    P3 = P3 * Scale;
    P4 = P4 * Scale;

    // Rotate world origin aligned rectangle points and then translate to rectangle origin aligned points
    vert_P1C1UV1 A, B, C, D;

    A.Position = vec3(CAng * P1.x - SAng * P1.y + T.x, SAng * P1.x + CAng * P1.y + T.y, Layer);
    A.UV = vec2(UV.x, UV.y);
    A.Color = Color;

    B.Position = vec3(CAng * P2.x - SAng * P2.y + T.x, SAng * P2.x + CAng * P2.y + T.y, Layer);
    B.UV = vec2(UV.z, UV.y);
    B.Color = Color;

    C.Position = vec3(CAng * P3.x - SAng * P3.y + T.x, SAng * P3.x + CAng * P3.y + T.y, Layer);
    C.UV = vec2(UV.z, UV.w);
    C.Color = Color;

    D.Position = vec3(CAng * P4.x - SAng * P4.y + T.x, SAng * P4.x + CAng * P4.y + T.y, Layer);
    D.UV = vec2(UV.x, UV.w);
    D.Color = Color;

    Vertices->push_back(A); Vertices->push_back(C); Vertices->push_back(D);
    Vertices->push_back(A); Vertices->push_back(B); Vertices->push_back(C);

    // When no rotation
    /*
    vert_P1C1UV1 A, B, C, D;

    A.Position = vec3(Dest.x, Dest.y, Layer);
    A.UV = vec2(UV.x, UV.y);
    A.Color = Color;

    B.Position = vec3(Dest.x + Dest.width, Dest.y, Layer);
    B.UV = vec2(UV.z, UV.y);
    B.Color = Color;

    C.Position = vec3(Dest.x + Dest.width, Dest.y + Dest.height, Layer);
    C.UV = vec2(UV.z, UV.y + UV.w);
    C.Color = Color;

    D.Position = vec3(Dest.x, Dest.y + Dest.height, Layer);
    D.UV = vec2(UV.x, UV.w);
    D.Color = Color;

    Vertices->push_back(A); Vertices->push_back(C); Vertices->push_back(D);
    Vertices->push_back(A); Vertices->push_back(B); Vertices->push_back(C);
    */
}

void PushSprite(std::vector<vert_P1C1UV1> *Vertices, rect Dest, vec4 UV, vec4 Color, r32 Rotation, vec2 Origin, r32 Layer)
{
    PushSprite(Vertices, Dest, UV, Color, Rotation, Origin, vec2(1.0f), Layer);
}

void PushText(std::vector<vert_P1C1UV1> *Vertices, rect Dest, vec4 Color, r32 Layer, font *Font, char const *Format, ...)
{
    char Text[8192];

    va_list ArgList;
    va_start(ArgList, Format);
    vsnprintf(Text, 8192, Format, ArgList);
    va_end(ArgList);

    vec2 Cursor = vec2(Dest.x, Dest.y);

    char *CharCurs = Text;
    
    while(*CharCurs != '\0')
    {
        if((u32)*CharCurs == 10) /* new line */
        {
            Cursor.x = Dest.x;
            Cursor.y = Cursor.y - Font->LineHeight;
            ++CharCurs;
            continue;
        }

        glyph *Glyph = FontGetGlyph(Font, (u32)*CharCurs);

        /*
            x,y                 x+w,y
            ---------------------
            -                   -
            -                   -
            -                   -
            --------------------- 
            x,y+h              x+w,y+h
        */
        vec4 TexCoord = vec4(Glyph->Rect.x / Font->ScaleW, 1.0f - (Glyph->Rect.y + Glyph->Rect.height) / Font->ScaleH,
                            (Glyph->Rect.x + Glyph->Rect.width) / Font->ScaleW, 1.0f - Glyph->Rect.y / Font->ScaleH);

        rect Dest = Rect(Cursor.x + Glyph->XOffset, Cursor.y - Glyph->YOffset - Glyph->Rect.height, Glyph->Rect.width, Glyph->Rect.height);
        
        PushSprite(Vertices, Dest, TexCoord, Color, 0.0f, vec2(0.0f), Layer);
        
        ++CharCurs;
        Cursor.x = Cursor.x + Glyph->XAdvance;
    }
}

void PushTextV(std::vector<vert_P1C1UV1> *Vertices, rect Dest, vec4 Color, r32 Layer, font *Font, char const *Format, va_list ArgList)
{
    char Text[8192];

    vsnprintf(Text, 8192, Format, ArgList);

    vec2 Cursor = vec2(Dest.x, Dest.y);

    char *CharCurs = Text;

    while(*CharCurs != '\0')
    {
        if((u32)*CharCurs == 10) /* new line */
        {
            Cursor.x = Dest.x;
            Cursor.y = Cursor.y - Font->LineHeight;
            ++CharCurs;
            continue;
        }

        glyph *Glyph = FontGetGlyph(Font, (u32)*CharCurs);

        /*
        x,y                 x+w,y
        ---------------------
        -                   -
        -                   -
        -                   -
        --------------------- 
        x,y+h              x+w,y+h
        */
        vec4 TexCoord = vec4(Glyph->Rect.x / Font->ScaleW, 1.0f - (Glyph->Rect.y + Glyph->Rect.height) / Font->ScaleH,
            (Glyph->Rect.x + Glyph->Rect.width) / Font->ScaleW, 1.0f - Glyph->Rect.y / Font->ScaleH);

        rect Dest = Rect(Cursor.x + Glyph->XOffset, Cursor.y - Glyph->YOffset - Glyph->Rect.height, Glyph->Rect.width, Glyph->Rect.height);

        PushSprite(Vertices, Dest, TexCoord, Color, 0.0f, vec2(0.0f), Layer);

        ++CharCurs;
        Cursor.x = Cursor.x + Glyph->XAdvance;
    }
}

//-----------------------------------------------------------------------------
// Debugging
//-----------------------------------------------------------------------------

void PushDbgLine(std::vector<vert_P1C1> *Vertices, vec3 FromP, vec3 ToP, vec4 Color)
{
    vert_P1C1 V1, V2;

    V1.Position = FromP;
    V1.Color = Color;
    V2.Position = ToP;
    V2.Color = Color;

    Vertices->push_back(V1); Vertices->push_back(V2);
}

void PushDbgRect(std::vector<vert_P1C1> *Vertices, rect Dest, vec4 Color, r32 Layer)
{
    /*
    0,1         1,1
    D-----------C
    |        /  |
    |      /    |
    |    /      |
    |  /        |
    A-----------B
    0,0         1,0
    (ACD)(ABC)
    */

    vert_P1C1 A, B, C, D;

    A.Position = vec3(Dest.x, Dest.y, Layer);
    A.Color = Color;

    B.Position = vec3(Dest.x + Dest.width, Dest.y, Layer);
    B.Color = Color;

    C.Position = vec3(Dest.x + Dest.width, Dest.y + Dest.height, Layer);
    C.Color = Color;

    D.Position = vec3(Dest.x, Dest.y + Dest.height, Layer);
    D.Color = Color;

    Vertices->push_back(A); Vertices->push_back(C); Vertices->push_back(D);
    Vertices->push_back(A); Vertices->push_back(B); Vertices->push_back(C);
}