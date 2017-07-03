#ifndef neon_texture_H
#define neon_texture_H
#include "neon_platform.h"
#include <cstdlib> // malloc, free
#include <cstring> // memset
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
////
////	Texture utility functions
////

// TGA file header format.
#pragma pack(push, 1)
typedef struct tga_header
{
  u8    IDLength;       
  u8    ColorMapType;   
  u8    ImageType;      
  u16   CMapStart;      
  u16   CMapLength;     
  u8    CMapDepth;      
  u16   XOffset;        
  u16   YOffset;        
  u16   Width;          
  u16   Height;         
  u8    PixelDepth;     
  u8    ImageDescriptor;
} tga_header;
#pragma pack(pop)


/**
 * texture
 *
 *	purpose: Load, rotate & free texture.
 */
class texture
{
public:
	s32 	Width;
	s32 	Height;
	void*	Content;
	u32  	ContentSize;
  	bool 	Initialised;
	
	void LoadFromFile(char const *Filename);
	void FlipVertically();
	void FreeMemory();

	texture();
	~texture();
};

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
////
////	TextureAltas functions
////
struct texture_coordinates
{
	r32 BL_X;
	r32 BL_Y;
	r32 TR_X;
	r32 TR_Y;
};

struct texture_rect
{
	u32 OriginX;
	u32 OriginY;
	
	u32 Width;
	u32 Height;
};

struct binary_t_node
{
	binary_t_node *Child[2];

	texture_rect Rect;
	bool Filled;
};

class texture_atlas
{
public:
	void	*Content;
	u32		ContentSize;
	u32		Width;
	u32		Height;
	u32		Padding;
	bool  	Initialised;

	binary_t_node Node;

	void Initialise(u32 AtlasWidth, u32 AtlasHeigth, u16 AtlasPadding);
	texture_coordinates PackTexture(texture *Texture);
	void FreeMemory();
	texture ToTexture();

	texture_atlas();
	~texture_atlas();
};

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
////
////	Texture Debugging functions
////
#ifdef DEBUG_BUILD
  #define DebugTextureSave(F, T) DebugTextureSave_(F, T)
#else
  #define DebugTextureSave(F, T)
#endif

void      DebugTextureSave_(char const * Filename, texture *Texture);


#endif