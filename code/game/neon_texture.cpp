#include "neon_texture.h"

#include "neon_renderer.h"

//-----------------------------------------------------------------------------
// Texture
//-----------------------------------------------------------------------------

texture::texture() :	InstanceID(GEN_ID),
						Width(0),
						Height(0),
						Content(0),
						ContentSize(0),
						FlippedAroundY(false),
						Type(texture_type::TEXTURE_2D),
						Filter(texture_filter::NEAREST),
						Wrap(texture_wrap::CLAMP),
						HwGammaCorrection(false),
						RenderResourceCreated(false)
		
{
}

texture::~texture()
{
	SAFE_FREE(Content);
}

void texture::LoadFile(char const * Filename, texture_type _Type, texture_filter _Filter, texture_wrap _Wrap, bool _HwGammaCorrection)
{
	file_content File = Platform.ReadFile(Filename);

	u8 *InBytes = (u8*)File.Content;

	u8 TGAType = *(u8 *)(InBytes + 2);

	// Check if the tga file type is what we want.
	// 2 == Uncompressed, True-color Image
	assert(TGAType == 2);

	// Assign members with width and height values from file
	Width = *(u16 *)(InBytes + 12);
	Height = *(u16 *)(InBytes + 14);

	// Read bit/pixel of file
	u8 BitsPerPixel = *(u8 *)(InBytes + 16);

	// Currently we only support 4 bytes per pixel only
	assert(BitsPerPixel == 32);

	// Set content size
	ContentSize = Width * Height * (BitsPerPixel / 8);

	// Allocate memory for pixel data
	Content = malloc(ContentSize);

	// Copy pixel data from file to our memory
	memcpy(Content, InBytes + sizeof(tga_header), ContentSize);

	/*
	GL			: RRGGBBAA == 0xAABBGGRR
	TGA ORDER	: AARRGGBB == 0xBBGGRRAA
	*/
	u32 *PixelPointer = (u32 *)Content;
	for(u32 PixelOffset = 0; PixelOffset < (ContentSize / 4); ++PixelOffset)
	{
		u32 *Pixel = (PixelPointer + PixelOffset);
		u32 B = (*Pixel & 0x000000FF) << 16;
		u32 G = (*Pixel & 0x0000FF00);
		u32 R = (*Pixel & 0x00FF0000) >> 16;
		u32 A = (*Pixel & 0xFF000000);
		*Pixel = R | G | B | A;
	}

	Platform.FreeFileContent(&File);

	FlippedAroundY = false;

	Type = _Type;
	Filter = _Filter;
	Wrap = _Wrap;
	HwGammaCorrection = _HwGammaCorrection;
}

bool texture::IsValid()
{
	return Width > 0 && Height > 0 && Content;
}

void texture::FreeContentMemory()
{
	SAFE_FREE(Content);
	ContentSize = 0;
}

void texture::FlipAroundY()
{
	assert(Content);

	FlippedAroundY = !FlippedAroundY;

	u32 *Data = (u32 *)malloc(ContentSize);

	memcpy(Data, Content, ContentSize);
	for(u32 Row = 0; Row < Height; ++Row)
	{
		for(u32 Col = 0; Col < Width; ++Col)
		{
			*((u32 *)(Content)+((Width) * (Height - 1 - Row)) + Col)
				= *(Data + (Width)* Row + Col);
		}
	}

	SAFE_FREE(Data);
}

void texture::CreateRenderResource()
{
	if(IsValid() && !RenderResourceCreated)
	{	
		RenderResource = rndr::MakeTexture(this);
		RenderResourceCreated = true;
	}
}

void texture::FreeRenderResource()
{
	if(RenderResourceCreated)
	{
		// TODO: Free texture render resource from GPU
		// rndr::FreeTexture()
		RenderResourceCreated = false;
	}
}

//-----------------------------------------------------------------------------
// Texture Debugging
//-----------------------------------------------------------------------------

void DebugTextureSave_(char const * Filename, texture *Texture)
{	
	tga_header Header = {};

	Header.IDLength = 0;       
	Header.ColorMapType = 0;   
	Header.ImageType = 2;      
	Header.CMapStart = 0;      
	Header.CMapLength = 0;     
	Header.CMapDepth = 0;      
	Header.XOffset = 0;        
	Header.YOffset = 0;        
	Header.Width = (u16)Texture->Width;          
	Header.Height = (u16)Texture->Height;         
	Header.PixelDepth = 32;     
	Header.ImageDescriptor = 0x20;
	
	if(!Texture->FlippedAroundY)
		Texture->FlipAroundY();

	// Change the colors byte position
	// Little-endian
	// Spec:   AA RR GG BB [ BB GG RR AA (On file) ]
	u32 *PixelPointer = (u32 *)malloc(Texture->ContentSize);
	
	memcpy(PixelPointer, Texture->Content, Texture->ContentSize);

	for(u32 PixelOffset = 0; PixelOffset < (Texture->ContentSize / 4); ++PixelOffset)
	{
		u32 *Pixel = (PixelPointer + PixelOffset);
	
		u32 R = (*Pixel & 0x000000FF) <<16;
		u32 G = (*Pixel & 0x0000FF00);
		u32 B = (*Pixel & 0x00FF0000) >> 16;
		u32 A = (*Pixel & 0xFF000000);
		*Pixel = A | R | G | B;
	}

	void *FileContent = malloc(sizeof(tga_header) + Texture->ContentSize);
	memcpy(FileContent, &Header, sizeof(tga_header));
	memcpy((u8 *)FileContent + sizeof(tga_header), PixelPointer, Texture->ContentSize);
	Platform.WriteFile(Filename, sizeof(tga_header) + Texture->ContentSize, FileContent);

	free(FileContent);
}

//-----------------------------------------------------------------------------
// TextureAtlas
//-----------------------------------------------------------------------------

texture_atlas::texture_atlas() :	Padding(0)
{
}

texture_atlas::~texture_atlas()
{
}

void texture_atlas::Init(u32 Width, u32 Height, u32 _Padding, texture_filter Filter, bool HwGammaCorrection)
{
	Texture.Type = texture_type::TEXTURE_2D;
	Texture.Wrap = texture_wrap::CLAMP;
	Texture.Filter = Filter;

	Texture.HwGammaCorrection = HwGammaCorrection;

	Texture.Width = Width;
	Texture.Height = Height;
	Padding = _Padding;

	Texture.Content = malloc(Texture.Width * Texture.Height * 4);
	if(Texture.Content == 0)
	{
		assert(!"Malloc error");
	}
	
	Texture.ContentSize = Texture.Width * Texture.Height * 4;

	memset(Texture.Content, 0, Texture.ContentSize);

	Node.Child[0] = 0;
	Node.Child[1] = 0;
	Node.Rect.OriginX = 1;
	Node.Rect.OriginY = 1;
	Node.Rect.Width = Texture.Width;
	Node.Rect.Height = Texture.Height;
	Node.Filled = false;

#if 0
	// make all pixels pink for debugging 
	u32 *Pixel = (u32 *)Content;	
	for(u32 x = 0; x < Width * Height; ++x)
	{
		*Pixel++ = 0xFFFF00FF; 
	}
#endif
}

bool texture_atlas::IsValid()
{
	return Texture.IsValid();
}

static inline binary_t_node* Atlas_Insert(binary_t_node *Node, texture *Texture, u32 Padding)
{
	// if we're not a leaf node
	if(Node->Child[0] != 0 && Node->Child[1] != 0)
	{
		binary_t_node *NewNode;

		// try inserting into first child
		NewNode = Atlas_Insert(Node->Child[0], Texture, Padding);
		
		// if new node is not null
		if(NewNode != 0)
		{
			return NewNode;
		}
		else
		{
			// no room in first child, try in second child
			return Atlas_Insert(Node->Child[1], Texture, Padding);
		}
	}
	// if we're a leaf node
	else
	{
		// if there's already a texture here, or the we're too small for the texture
		if((Node->Filled) || (Node->Rect.Width < Texture->Width + Padding) || 
			(Node->Rect.Height < Texture->Height + Padding))
		{

			return 0;
		}

		// if we're just right size
		if((Node->Rect.Width == Texture->Width + Padding) && 
			(Node->Rect.Height == Texture->Height + Padding))
		{
			Node->Filled = true;

			/*Platform->Log(INFO, "Node O_x=%d O_y=%d W=%d H=%d\n", Node->Rect.OriginX,Node->Rect.OriginY,
														Node->Rect.Width, Node->Rect.Height);
			*/
			return Node;
		}

		Node->Child[0] = (binary_t_node *)malloc(sizeof(binary_t_node));
		Node->Child[1] = (binary_t_node *)malloc(sizeof(binary_t_node));
		
		Node->Child[0]->Filled = false;
		Node->Child[1]->Filled = false;

		Node->Child[0]->Child[0] = 0;
		Node->Child[0]->Child[1] = 0;
		Node->Child[1]->Child[0] = 0;
		Node->Child[1]->Child[1] = 0;


		// decide which way to split
		u32 dw = Node->Rect.Width  - (Texture->Width + Padding);
		u32 dh = Node->Rect.Height - (Texture->Height + Padding);

		if(dw > dh)
		{
			// divide vertically
			Node->Child[0]->Rect.OriginX = Node->Rect.OriginX;
			Node->Child[0]->Rect.OriginY = Node->Rect.OriginY;
			Node->Child[0]->Rect.Width	 = (Texture->Width + Padding);
			Node->Child[0]->Rect.Height  = Node->Rect.Height;
			
			Node->Child[1]->Rect.OriginX = Node->Rect.OriginX + (Texture->Width + Padding);
			Node->Child[1]->Rect.OriginY = Node->Rect.OriginY;
			Node->Child[1]->Rect.Width	 = Node->Rect.Width - (Texture->Width + Padding);
			Node->Child[1]->Rect.Height  = Node->Rect.Height;

			return Atlas_Insert(Node->Child[0], Texture, Padding);
		}
		else
		{
			// divide horizontally
			Node->Child[0]->Rect.OriginX = Node->Rect.OriginX;
			Node->Child[0]->Rect.OriginY = Node->Rect.OriginY;
			Node->Child[0]->Rect.Width	 = (Texture->Width + Padding);
			Node->Child[0]->Rect.Height  = (Texture->Height + Padding);
			
			Node->Child[1]->Rect.OriginX = Node->Rect.OriginX;
			Node->Child[1]->Rect.OriginY = Node->Rect.OriginY + (Texture->Height + Padding);
			Node->Child[1]->Rect.Width	 = Node->Rect.Width;
			Node->Child[1]->Rect.Height  = Node->Rect.Height - (Texture->Height + Padding);

			return Atlas_Insert(Node->Child[0], Texture, Padding);
		}
	}
}

texture_coords texture_atlas::Pack(texture *_Texture)
{
	assert(IsValid());

	binary_t_node *NodeSlot = Atlas_Insert(&Node, _Texture, Padding);

	if(NodeSlot == 0)
	{
		assert(!"Null returned.");
	}

	texture_coords TCoord;

	//TCoord.BL_X = (r32)(NodeSlot->Rect.OriginX - 1.5) / (r32)(Width - 1);
	//TCoord.BL_Y = (r32)(NodeSlot->Rect.OriginY + NodeSlot->Rect.Height - Padding - 1.5) / (r32)(Height - 1);
	//TCoord.TR_X = (r32)(NodeSlot->Rect.OriginX + NodeSlot->Rect.Width - Padding - 1.5) / (r32)(Width - 1);
	//TCoord.TR_Y = (r32)(NodeSlot->Rect.OriginY - 1.5) / (r32)(Height - 1);

	TCoord.LowerLeft.x	= (r32)(NodeSlot->Rect.OriginX - 1.0) / (r32)(Texture.Width - 0);
	TCoord.LowerLeft.y	= 1.0f - (r32)(NodeSlot->Rect.OriginY + NodeSlot->Rect.Height - Padding - 1.0) / (r32)(Texture.Height - 0);
	TCoord.UpperRight.x	= (r32)(NodeSlot->Rect.OriginX + NodeSlot->Rect.Width - Padding - 1.0) / (r32)(Texture.Width - 0);
	TCoord.UpperRight.y = 1.0f - (r32)(NodeSlot->Rect.OriginY - 1.0) / (r32)(Texture.Height - 0);


	// Platform->Log(INFO, "BL_x=%f BL_y=%f TR_x=%f TR_y=%f\n", TCoord.BL_X, TCoord.BL_Y, TCoord.TR_X, TCoord.TR_Y);

	// Copy the texture on the atlas at its position.
	// x and y in range [1, width and height] that
	// needs to be written in range [0, Width - 1 or height - 1]
	// write_at(x, y) = (u32)content_ptr + (x-1) + ((y-1) * width)
	u32 *pTextureContent = (u32 *)_Texture->Content;
	
	for(u32 y = NodeSlot->Rect.OriginY; y < NodeSlot->Rect.OriginY + NodeSlot->Rect.Height - Padding; ++y)
	{
		for(u32 x = NodeSlot->Rect.OriginX; x < NodeSlot->Rect.OriginX + NodeSlot->Rect.Width - Padding; ++x)
		{
			u32 *Pixel = (u32 *)Texture.Content + (x - 1) + ((y-1) * Texture.Width);
			*Pixel = *(pTextureContent);
			pTextureContent++;
		}
	}

	 //this->GenTexture();
	 //DebugTextureSave("FontAtlas.tga", &Texture);

	return TCoord;
}