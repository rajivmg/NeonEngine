#ifndef NEON_TEXTURE_H
#define NEON_TEXTURE_H

#include "../platform/neon_platform.h"
#include "neon_math.h"

#include <cstdlib> // malloc

#include "neon_renderer.h"

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

//-----------------------------------------------------------------------------
// Texture
//-----------------------------------------------------------------------------

enum class texture_type
{
	XX,
	TEXTURE_2D
};

enum class texture_filter
{
	XX,
	LINEAR,
	NEAREST,
};

enum class texture_wrap
{
	XX,
	CLAMP,
	REPEAT
};

struct bitmap
{
	u32		Width;
	u32		Height;
	void	*Data;
	u32		DataSize;
	u32		BytesPerPixel;
	bool	FlippedAroundY;
};

void LoadBitmap(bitmap *Bitmap, char const *Filename);
void FreeBitmap(bitmap *Bitmap);
void BitmapFlipAroundY(bitmap *Bitmap);

class texture
{
public:
	u32 const		InstanceID;
	u32				Width;
	u32				Height;
	void			*Content;
	u32				ContentSize;

	bool			FlippedAroundY;

	texture_type	Type;
	texture_filter	Filter;
	texture_wrap	Wrap;

	// If true this texture will be setup so that on sampling it
	// hardware gamma correction is applied i.e. it's converted from sRGB to linear space.
	// NOTE: Must be set before 'CreateRenderResource'.
	bool			HwGammaCorrection;

	render_resource RenderResource;
	bool			RenderResourceCreated;

	// Load texture from .tga file.
	// NOTE: Pixels data origin is top left of the image(texture).
	void			LoadFile(char const *Filename, texture_type _Type, texture_filter _Filter, texture_wrap _Wrap, bool _HwGammaCorrection = false);

	// Returns true if texture is valid
	bool			IsValid();

	// Frees the pixel data memory.
	void			FreeContentMemory();

	// Flip the texture vertically.
	void			FlipAroundY();

	// Creates the render resource for this texture.
	// NOTE: This must not be called before texture is initialised.
	void			CreateRenderResource();
	
	// Frees the render resource for this texture.
	void			FreeRenderResource();

	texture();
	~texture();
};

//-----------------------------------------------------------------------------
// TextureAtlas
//-----------------------------------------------------------------------------

struct texture_coords
{
	vec2 LowerLeft;
	vec2 UpperRight;
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
	texture			Texture;		
	u32				Padding;

	binary_t_node	Node;

	void			Init(u32 Width, u32 Height, u32 _Padding, texture_filter Filter, bool HwGammaCorrection = false);

	// Returns true if texture atlas is valid
	bool			IsValid();

	// Returns the texture coordinates of the given texture in the texture atlas.
	// Note that the coordinates will be in OGL style. i.e. (0,0) = Bottom Left
	texture_coords	Pack(texture *_Texture);

	texture_atlas();
	~texture_atlas();
};

//-----------------------------------------------------------------------------
// Texture Debugging
//-----------------------------------------------------------------------------

#ifdef DEBUG
  #define DebugTextureSave(F, T) DebugTextureSave_(F, T)
#else
  #define DebugTextureSave(F, T)
#endif

// Save texture object on disk in .tga format
// Note that the first byte of texture data will be saved as the first top left pixel.
void DebugTextureSave_(char const *Filename, texture *Texture);

#endif