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
	P1UV1C1
};

struct vert_P1UV1C1
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

	struct idraw
	{
		render_resource		IndexBuffer;
		render_resource		VertexBuffer;
		vert_format			VertexFormat;
		u32					StartVertex;
		u32					IndexCount;
		texture				*Texture;
		render_resource		ShaderProgram;

		static const dispatch_fn DISPATCH_FUNCTION;
	};
	static_assert(std::is_pod<idraw>::value == true, "Must be a POD.");
}

void PushTextSprite(std::vector<vert_P1UV1C1> *Vertices, font *Font, vec3 P, vec4 Color, char const * Format, ...);

#endif