#include "neon_renderer.h"

#include "neon_font.h"
#include "neon_opengl.h"
#include "neon_primitive_mesh.h"

const dispatch_fn cmd::udraw::DISPATCH_FUNCTION = &rndr::UnindexedDraw;

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

render_resource rndr::MakeTexture(texture * Texture)
{
	return ogl::MakeTexture(Texture);
}

render_resource rndr::MakeVertexBuffer(u32 Size, bool Dynamic)
{
	return ogl::MakeVertexBuffer(Size, Dynamic);
}

void rndr::VertexBufferData(render_resource VertexBuffer, u32 Offset, u32 Size, void const *Data)
{
	ogl::VertexBufferData(VertexBuffer, Offset, Size, Data);
}

void rndr::DeleteVertexBuffer(render_resource VertexBuffer)
{
	ogl::DeleteVertexBuffer(VertexBuffer);
}

render_resource rndr::MakeIndexBuffer(u32 Size, bool Dynamic)
{
	return ogl::MakeIndexBuffer(Size, Dynamic);
}

void rndr::IndexBufferData(render_resource IndexBuffer, u32 Offset, u32 Size, void const *Data)
{
	ogl::IndexBufferData(IndexBuffer, Offset, Size, Data);
}

void rndr::DeleteIndexBuffer(render_resource IndexBuffer)
{
	ogl::DeleteIndexBuffer(IndexBuffer);
}

render_resource rndr::MakeShaderProgram(char const *VertShaderSrc, char const *FragShaderSrc)
{
	return ogl::MakeShaderProgram(VertShaderSrc, FragShaderSrc);
}

void rndr::DeleteShaderProgram(render_resource ShaderProgram)
{
	ogl::DeleteShaderProgram(ShaderProgram);
}

void rndr::UnindexedDraw(void const *Data)
{
	cmd::udraw *Cmd = (cmd::udraw *)Data;
	ogl::UnindexedDraw(Cmd);
	//Platform->Log(INFO, "TexID = %d\n", Cmd->TexID);

}

//-----------------------------------------------------------------------------
// Render Commands
//-----------------------------------------------------------------------------

render_cmd_list::render_cmd_list(u32 _BufferSize) : BufferSize(_BufferSize),
													BaseOffset(0),
													Current(0)
{
	const u32 MaxPacketsInList = 2048;

	Buffer = malloc(BufferSize);
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
	Current = 0;
	BaseOffset = 0;
}

void PushTextSprite(std::vector<vert_P1UV1C1> *Vertices, font *Font, vec3 P, vec4 Color, char const * Format, ...)
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
