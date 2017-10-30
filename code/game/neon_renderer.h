#ifndef NEON_RENDERER_H
#define NEON_RENDERER_H

#include "../platform/neon_platform.h"
#include "neon_math.h"

#include <vector>

class texture;
class font;
enum class texture_type;
enum class texture_filter;
enum class texture_wrap;

typedef void (*dispatch_fn)(void const *Data);

//
//	NOTE:
//	- Follow right-handed coordinate system.
//	
//	- Counter-clockwise vertex winding = triangle's front.
//	 
//
//	 			 	(+Y)			
//	 			 	|   / (-Z)(Inward)
//	 			 	|  /		
//	 			 	| /
//	 			 	|/
//	 	(-X) --------O--------- (+X)
//	 			   /|	
//	 			  / |
//	 			 /  |					(3D)
//	 	 (+Z)	/   |
//	  	(Outward)	(-Y)
//
//
//		+Y
//		|
//		|
//		|
//		|
//		|								(2D) 
//		|
//		| 
// 		O---------------- +X
//
//
//

//-----------------------------------------------------------------------------
// Renderer Wrapper
//-----------------------------------------------------------------------------

struct render_resource
{
	enum resource_type : u32
	{
		VERTEX_BUFFER, INDEX_BUFFER, SHADER_PROGRAM, TEXTURE, RENDER_TARGET,
		NOT_INITIALIZED = 0xFFFFFFFF
	};

	resource_type	Type;
	u32				ResourceHandle;
};

namespace rndr
{
	void			Init();
	void			Clear();
	
	render_resource MakeTexture(texture *Texture);
	
	render_resource	MakeVertexBuffer(u32 Size, bool Dynamic = true);
	void			VertexBufferData(render_resource VertexBuffer, u32 Offset, u32 Size, void const *Data);
	void			DeleteVertexBuffer(render_resource VertexBuffer);
	
	render_resource	MakeIndexBuffer(u32 Size, bool Dynamic = true);
	void			IndexBufferData(render_resource IndexBuffer, u32 Offset, u32 Size, void const *Data);
	void			DeleteIndexBuffer(render_resource IndexBuffer);

	render_resource MakeShaderProgram(char const *VertShaderSrc, char const *FragShaderSrc);
	void			DeleteShaderProgram(render_resource ShaderProgram);

	void			UnindexedDraw(void const *Data);
}

/*
struct render_target
{
u32 Handle;
ivec2 Size;
};
extern render_target RT0;
render_target	MakeRenderTarget(ivec2 Size, texture_filter Filter);
*/

//-----------------------------------------------------------------------------
// Render Commands
//-----------------------------------------------------------------------------

struct cmd_packet
{
	cmd_packet		*NextCmdPacket;
	dispatch_fn		DispatchFn;
	void			*Cmd;
	void			*AuxMemory;
};

// TODO: We don't need a template here but for now let it be.
template <typename T>
inline cmd_packet* GetCmdPacket(T *Cmd)
{
	return (cmd_packet *)((u8 *)Cmd - sizeof(cmd_packet));
}

struct render_cmd_list
{
	cmd_packet **Packets;	// List of packets
	u32		*Keys;			// Keys of packets
	u32		Current;		// Number of packets in the list
	void	*Buffer;		// Memory buffer
	u32		BufferSize;		// Size of memory buffer in bytes
	u32		BaseOffset;		// Number of bytes used in the memory buffer
	
	// TODO: Add camera info and render target info

	render_cmd_list(u32 _BufferSize);
	~render_cmd_list();

	template <typename U>
	u32 GetCmdPacketSize(u32 AuxMemorySize);

	// Create a cmd_packet and initialise it's members.
	template <typename U>
	cmd_packet* CreateCmdPacket(u32 AuxMemorySize);

	template <typename U> 
	U* AddCommand(u32 Key, u32 AuxMemorySize);
	
	template <typename U, typename V> 
	U* AppendCommand(V *Cmd, u32 AuxMemorySize);
	
	void* AllocateMemory(u32 MemorySize);
	void Sort();
	void Submit();
	void Flush();
};

template <typename U>
inline u32 render_cmd_list::GetCmdPacketSize(u32 AuxMemorySize)
{
	return sizeof(cmd_packet) + sizeof(U) + AuxMemorySize;
}

template <typename U>
inline cmd_packet* render_cmd_list::CreateCmdPacket(u32 AuxMemorySize)
{
	cmd_packet *Packet = (cmd_packet *)AllocateMemory(GetCmdPacketSize<U>(AuxMemorySize));
	Packet->NextCmdPacket = nullptr;
	Packet->DispatchFn = nullptr;
	Packet->Cmd = (u8 *)Packet + sizeof(cmd_packet);
	Packet->AuxMemory = AuxMemorySize > 0 ? (u8 *)Packet->Cmd + sizeof(U) : nullptr;

	return Packet;
}

template <typename U>
inline U* render_cmd_list::AddCommand(u32 Key, u32 AuxMemorySize)
{
	cmd_packet *Packet = CreateCmdPacket<U>(AuxMemorySize);

	const u32 I = Current++;
	Packets[I] = Packet;
	Keys[I] = Key; 

	Packet->NextCmdPacket = nullptr;
	Packet->DispatchFn = U::DISPATCH_FUNCTION;

	return (U *)(Packet->Cmd);
}

template <typename U, typename V>
inline U* render_cmd_list::AppendCommand(V *Cmd, u32 AuxMemorySize)
{
	cmd_packet *Packet = CreateCmdPacket<U>(AuxMemorySize);
	
	GetCmdPacket<V>(Cmd)->NextCmdPacket = Packet;
	Packet->NextCmdPacket = nullptr;
	Packet->DispatchFn = U::DISPATCH_FUNCTION;

	return (U *)(Packet->Cmd);
}

enum class vert_format
{
	POS3UV2COLOR4
};

struct vert_POS3UV2COLOR4
{
	vec3 Position;
	vec2 UV;
	vec4 Color;
};

namespace cmd
{
	struct udraw
	{
		render_resource		VertexBuffer;
		vert_format			VertexFormat;
		u32					StartVertex;
		u32					VertexCount;
		texture				*Texture;
		render_resource		ShaderProgram;

		static const dispatch_fn DISPATCH_FUNCTION;
	};
	static_assert(std::is_pod<udraw>::value == true, "Must be a POD.");


	/*struct idraw
	{
		render_resource	Texture;
		render_resource	VertexBuffer;
		render_resource	IndexBuffer;
		u32				StartVertex;
		u32				IndexCount;

		static const dispatch_fn DISPATCH_FUNCTION;
	};
	static_assert(std::is_pod<idraw>::value == true, "Must be a POD.");*/
}

/*
enum render_cmd_type : u8
{
	RenderCmd_Clear,
	RenderCmd_ColorQuad,
	RenderCmd_TextureQuad,
	RenderCmd_Line,
	RenderCmd_Text,
	RenderCmd_RenderTarget
};

struct render_cmd_header
{
	union
	{
		u32 Key;
		struct
		{
			u16 Texture;
			render_cmd_type Type;	
			u8 Target;
		};
	};
};

struct render_cmd
{
	render_cmd_header Header;
};

struct render_cmd_Clear
{
	render_cmd_header Header;
};

struct render_cmd_Line
{
	render_cmd_header Header;
	vec3 Start;
	vec3 End;
	vec4 Color;
};

struct render_cmd_TextureQuad
{
	render_cmd_header Header;
	u32  TextureIndex;
	vec3 P;
	vec2 Size;
	vec4 UV;
	vec4 Tint;
};

struct render_cmd_ColorQuad
{
	render_cmd_header Header;
	vec3 P;
	vec2 Size;
	vec4 Color;
};

struct render_cmd_Text
{
	render_cmd_header Header;
	font *Font;
	vec3 P;
	vec4 Color;
	char Text[8192];
};

struct render_cmd_RenderTarget
{
	render_cmd_header Header;
	vec3 P;
	vec2 Size;
	render_target SrcRenderTarget;
	render_target DestRenderTarget;
};

struct render_cmd_list
{
	void *List;
	u32 BaseOffset;
	u32 Size;
	
	void **Table;
	u32 CmdCount;

	void *Scratch;
};

render_cmd_list* AllocRenderCmdList();
void PushRenderCmd(render_cmd_list *RenderCmdList, void *RenderCmd);
void SortRenderCmdList(render_cmd_list *RenderCmdList);
void DrawRenderCmdList(render_cmd_list *RenderCmdList);

void RenderCmdClear(render_cmd_list *RenderCmdList);
void RenderCmdLine(render_cmd_list *RenderCmdList, vec3 aStart, vec3 aEnd, vec4 aColor, render_target RenderTarget);
void RenderCmdTextureQuad(render_cmd_list *RenderCmdList, vec3 aP, vec2 aSize, vec4 aUV, vec4 aTint, u32 aTextureIndex, render_target RenderTarget);
void RenderCmdColorQuad(render_cmd_list *RenderCmdList, vec3 aP, vec2 aSize, vec4 aColor, render_target RenderTarget);
void RenderCmdText(render_cmd_list *RenderCmdList, vec3 aP, vec4 aColor, font *aFont, render_target RenderTarget, char const *Fmt, ...);
void RenderCmdRenderTarget(render_cmd_list *RenderCmdList, vec3 aP, vec2 aSize, render_target SrcRenderTarget, render_target DestRenderTarget);
*/
#endif