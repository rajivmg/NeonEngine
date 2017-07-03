#include "neon_texture.h"

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
////
////	Texture utility functions
////
texture::texture()
{
	Initialised = false;
	Content = 0;
}

texture::~texture()
{
}

/**
 * texture::LoadFromFile
 * 
 * purpose: Load texture data from disk into the texture object.
 * 
 * description: Considers the first byte of the texture data(on disk) as
 *				very first top left pixel of the texture. 
 */
void texture::LoadFromFile(char const *Filename)
{
	read_file_result Data = Platform->ReadFile(Filename);

	u8 *InBytes = (u8*)Data.Content;

	u8 TGAType = *(u8 *)(InBytes + 2);
	Assert(TGAType == 2); // @TODO: Error message 

	Width  = *(u16 *)(InBytes + 12);
	Height = *(u16 *)(InBytes + 14);

	u8 BitsPerPixel = *(u8 *)(InBytes + 16);
	Assert(BitsPerPixel == 32); // @TODO: Error message

	ContentSize = Width * Height * (32/8); // bits/pixel = 32

	Content = malloc(ContentSize);
	
	memcpy(Content, InBytes + sizeof(tga_header), ContentSize);
	
	Platform->FreeFileMemory(&Data);

	// Little-endian 
	// On disk : 		  BB GG RR AA
	// In Mem  : 		  AA RR GG BB (Disk BBGGRRAA)
	// After Processing : RR GG BB AA (Mem 0xAABBGGRR)
	u32 *PixelPointer = (u32 *)Content;
	for(u32 PixelOffset = 0; PixelOffset < (ContentSize / 4); ++PixelOffset)
	{
		u32 *Pixel = (PixelPointer + PixelOffset);
		u32 B = (*Pixel & 0x000000FF) << 16;
		u32 G = (*Pixel & 0x0000FF00) ;//<< 8;
		u32 R = (*Pixel & 0x00FF0000) >> 16;
		u32 A = (*Pixel & 0xFF000000);// >> 24;
		*Pixel = R | G | B | A;
	}

	Initialised = true;
}

void texture::FlipVertically()
{
	Assert(Initialised);

	u32 *Data = (u32 *)malloc(ContentSize);

	memcpy(Data, Content, ContentSize);
	for(int Row = 0; Row < Height; ++Row)
	{
		for(int Col = 0; Col < Width; ++Col)
		{	
			*((u32 *)(Content) + ((Width) * (Height - 1 - Row)) + Col)
				= *(Data + (Width) * Row + Col); 
		}
	}

	free(Data);
}

void texture::FreeMemory()
{
	Assert(Initialised);
	SAFE_FREE(Content);
}

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
////
////	Texture debugging functions
////

/**
 * DebugTextureSave
 *
 * purpose: Save the texture objects as .tga file on disk
 *
 * description: Considers the first byte of the texture->content as very
 *				first top left pixel of the texture. 
 */
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
	
	// Change the colors byte position
	// Little-endian
	// Spec:   AA RR GG BB [ BB GG RR AA (On file) ]
	u32 *PixelPointer = (u32 *)Texture->Content;
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
	memcpy((u8 *)FileContent + sizeof(tga_header), Texture->Content, Texture->ContentSize);
	Platform->WriteFile(Filename, sizeof(tga_header) + Texture->ContentSize, FileContent);

	free(FileContent);
}

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
////
////	TextureAtlas functions
////

texture_atlas::texture_atlas()
{
	Initialised = false;
	Content = 0;
}

texture_atlas::~texture_atlas()
{
}

void texture_atlas::Initialise(u32 AtlasWidth, u32 AtlasHeigth, u16 AtlasPadding)
{
	Width = AtlasWidth;
	Height = AtlasHeigth;
	Padding = AtlasPadding;

	Content = malloc(Width * Height * 4);
	if(Content == 0)
	{
		Assert(!"Error");
	}
	
	ContentSize = Width * Height * 4;

	memset(Content, 0, ContentSize);

	Node.Child[0] = 0;
	Node.Child[1] = 0;
	Node.Rect.OriginX = 1;
	Node.Rect.OriginY = 1;
	Node.Rect.Width = Width;
	Node.Rect.Height = Height;
	Node.Filled = false;
	Initialised = true;

#if 1
	// make all pixels pink for debugging 
	u32 *Pixel = (u32 *)Content;	
	for(u32 x = 0; x < Width * Height; ++x)
	{
		*Pixel++ = 0xFFFF00FF; 
	}
#endif
}

static binary_t_node* Atlas_Insert(binary_t_node *Node, texture *Texture, u32 Padding)
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

texture_coordinates texture_atlas::PackTexture(texture *Texture)
{
	Assert(Initialised == true);

	binary_t_node *NodeSlot = Atlas_Insert(&Node, Texture, Padding);

	if(NodeSlot == 0)
	{
		Assert(!"Null returned.");
	}

	texture_coordinates TCoord;

	TCoord.BL_X = (r32)(NodeSlot->Rect.OriginX - 1.5) / (r32)(Width - 1);
	TCoord.BL_Y = (r32)(NodeSlot->Rect.OriginY + NodeSlot->Rect.Height - Padding - 1.5) / (r32)(Height - 1);
	TCoord.TR_X = (r32)(NodeSlot->Rect.OriginX + NodeSlot->Rect.Width - Padding - 1.5) / (r32)(Width - 1);
	TCoord.TR_Y = (r32)(NodeSlot->Rect.OriginY - 1.5) / (r32)(Height - 1);

	// Platform->Log(INFO, "BL_x=%f BL_y=%f TR_x=%f TR_y=%f\n", TCoord.BL_X, TCoord.BL_Y, TCoord.TR_X, TCoord.TR_Y);

	// copy the texture on the atlas at its position
	// x and y in range [1, width or height]
	// needs to be written in range [0, Width or height - 1]
	// write_at(x, y) = (u32)content_ptr + (x-1) + ((y-1) * width)
	u32 *pTextureContent = (u32 *)Texture->Content;
	
	for(u32 y = NodeSlot->Rect.OriginY; y < NodeSlot->Rect.OriginY + NodeSlot->Rect.Height - Padding; ++y)
	{
		for(u32 x = NodeSlot->Rect.OriginX; x < NodeSlot->Rect.OriginX + NodeSlot->Rect.Width - Padding; ++x)
		{
			u32 *Pixel = (u32 *)Content + (x - 1) + ((y-1) * Width);
			*Pixel = *(pTextureContent);
			pTextureContent++;
		}
	}
	
	// Debugging purpose
	// texture AtlasDebugCopy;
	// AtlasDebugCopy = this->ToTexture();
	// DebugTextureSave("FontAtlas.tga", &AtlasDebugCopy);

	return TCoord;
}

void texture_atlas::FreeMemory()
{
	Assert(Initialised);
	SAFE_FREE(Content);
}

texture texture_atlas::ToTexture()
{
	texture Texture;

	Texture.Width  = Width;
	Texture.Height = Height;
	Texture.ContentSize = ContentSize;
	Texture.Content = malloc(ContentSize);
	memcpy(Texture.Content, Content, ContentSize);
	Texture.Initialised = true;
	
	return Texture;
}
