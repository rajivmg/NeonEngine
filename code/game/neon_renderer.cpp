#include "neon_renderer.h"

#include "neon_text.h"
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
    // Initialise opengl function pointers.
    InitGL();

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

render_resource rndr::MakeTexture(bitmap *Bitmap, texture_type Type, texture_filter Filter, texture_wrap Wrap, bool HwGammaCorrection)
{
    return ogl::MakeTexture(Bitmap, Type, Filter, Wrap, HwGammaCorrection);
}

void rndr::DeleteTexture(render_resource Texture)
{
    ogl::DeleteTexture(Texture);
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

render_cmd_list::render_cmd_list(u32 _BufferSize, render_resource _ShaderProgram) :
    BufferSize(_BufferSize),
    ShaderProgram(_ShaderProgram),
    BaseOffset(0),
    Current(0)
{
    const u32 MaxPacketsInList = 2048;

    Buffer = malloc(BufferSize);
    memset(Buffer, 0, BufferSize);
    Keys = (u32 *)malloc(sizeof(u32) * MaxPacketsInList);
    Packets = (cmd_packet **)malloc(sizeof(cmd_packet *) * (MaxPacketsInList));
}

render_cmd_list::~render_cmd_list()
{
    SAFE_FREE(Buffer)
}

void* render_cmd_list::AllocateMemory(u32 MemorySize)
{
    assert((BufferSize - BaseOffset) >= MemorySize);
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
    rndr::SetViewMatrix(ViewMatrix);
    rndr::SetProjectionMatrix(ProjMatrix);
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

void PushSprite(std::vector<vert_P1C1UV1> *Vertices, vec3 P, vec2 Size, vec4 Color, vec4 UV)
{
    /*
    D--------C
    |  U    /|
    |      / |
    |     /  |
    |    /   |
    |   /    |
    |  /     |
    | /      |
    |/    B  |
    A--------B

    A.XYZ = Origin.XYZ
    A.UV  = UVCoords.xY

    B.X   = Origin.X + Size.x
    B.Y   = Origin.Y
    B.Z   = 0
    B.U   = UVCoords.z
    B.V   = UVCoords.y

    C.X   = Origin.X + Size.x
    C.Y   = Origin.Y + Size.y
    C.Z   = 0
    C.UV  = UVCoords.zW

    D.X   = Origin.X
    D.Y   = Origin.Y + Size.y
    D.Z   = 0
    D.U   = UVCoords.x
    D.V   = UVCoords.w
    */

    // NOTE: Enabling this in debug build increase the frametime 5 times
    //Vertices->reserve(Vertices->size() + 6);

    vert_P1C1UV1 Vertex;

    // D
    Vertex.Position = vec3(P.x, P.y + Size.y, P.z);
    Vertex.UV = vec2(UV.x, UV.w);
    Vertex.Color = Color;
    Vertices->push_back(Vertex);

    // A
    Vertex.Position = P;
    Vertex.UV = vec2(UV.x, UV.y);
    Vertex.Color = Color;
    Vertices->push_back(Vertex);

    // C
    Vertex.Position = vec3(P.x + Size.x, P.y + Size.y, P.z);
    Vertex.UV = vec2(UV.z, UV.w);
    Vertex.Color = Color;
    Vertices->push_back(Vertex);

    // C 
    Vertex.Position = vec3(P.x + Size.x, P.y + Size.y, P.z);
    Vertex.UV = vec2(UV.z, UV.w);
    Vertex.Color = Color;
    Vertices->push_back(Vertex);

    // A
    Vertex.Position = P;
    Vertex.UV = vec2(UV.x, UV.y);
    Vertex.Color = Color;
    Vertices->push_back(Vertex);

    // B
    Vertex.Position = vec3(P.x + Size.x, P.y, P.z);
    Vertex.UV = vec2(UV.z, UV.y);
    Vertex.Color = Color;
    Vertices->push_back(Vertex);
}

void PushLine(std::vector<vert_P1C1> *Vertices, vec3 FromP, vec3 ToP, vec4 Color)
{
    vert_P1C1 Vertex;

    Vertex.Position = FromP;
    Vertex.Color = Color;
    Vertices->push_back(Vertex);

    Vertex.Position = ToP;
    Vertex.Color = Color;
    Vertices->push_back(Vertex);
}

void PushTextSprite(std::vector<vert_P1C1UV1> *Vertices, font *Font, vec3 P, vec4 Color, char const *Format, ...)
{
    char Text[8192];

    va_list ArgList;
    va_start(ArgList, Format);
    vsnprintf(Text, 8192, Format, ArgList);
    va_end(ArgList);

    vec3 Pen = P;
    Pen.y -= Font->Height;

    u32 CharCount = 0;
    int Index = 0;
    while(Text[Index] != 0)
    {
        // If the character is new line.
        if((int)Text[Index] == 10)
        {
            Pen.x = P.x;
            Pen.y -= Font->Height;
            ++Index;
            continue;
        }

        // If the character is other than a new line
        glyph *CharGlyph = Font->Glyphs + ((int)Text[Index] - 32);
        vec4 TexCoords = vec4(CharGlyph->Coords.LowerLeft.x, CharGlyph->Coords.LowerLeft.y,
            CharGlyph->Coords.UpperRight.x, CharGlyph->Coords.UpperRight.y);
        vec3 CharOrigin = vec3(Pen.x + CharGlyph->HoriBearingX, Pen.y + CharGlyph->Hang, Pen.z);

        PushSprite(Vertices,
            CharOrigin,
            vec2i(CharGlyph->Width, CharGlyph->Height),
            Color,
            TexCoords);

        ++CharCount;

        Pen.x += CharGlyph->HoriAdvance;

        ++Index;
    }
}
