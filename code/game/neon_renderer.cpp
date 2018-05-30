#include "neon_renderer.h"

#include "neon_font.h"
#include "neon_opengl.h"
#include "neon_primitive_mesh.h"

const dispatch_fn cmd::draw::DISPATCH_FUNCTION = &rndr::Draw;
const dispatch_fn cmd::draw_indexed::DISPATCH_FUNCTION = &rndr::DrawIndexed;
const dispatch_fn cmd::copy_const_buffer::DISPATCH_FUNCTION = &rndr::CopyConstBuffer;

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

#if 0
render_resource rndr::MakeTexture(texture *Texture)
{
	return ogl::MakeTexture(Texture);
}
#endif

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
		while((j > 0) &&  Keys[j - 1] > Keys[j])
		{
			cmd_packet *Temp = Packets[j];
			Packets[j] = Packets[j-1];
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
			dispatch_fn DispatchFn = Packet->DispatchFn;
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

void PushTextSprite(std::vector<vert_P1C1UV1> *Vertices, font *Font, vec3 P, vec4 Color, char const *Format, ...)
{
	char Text[8192];

	va_list ArgList;
	va_start(ArgList, Format);
	vsnprintf(Text, 8192, Format, ArgList);
	va_end(ArgList);

	vec3 Pen = P;
	Pen.y -= Font->FontHeight;

	u32 CharCount = 0;
	int Index = 0;
	while(Text[Index] != 0)
	{
		// If the character is new line.
		if((int)Text[Index] == 10)
		{
			Pen.x = P.x;
			Pen.y -= Font->FontHeight;
			++Index;
			continue;
		}

		// If the character is other than a new line
		glyph *CharGlyph = Font->Glyphs + ((int)Text[Index] - 32);
		vec4 TexCoords	= vec4(CharGlyph->Coords.LowerLeft.x, CharGlyph->Coords.LowerLeft.y, 
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
