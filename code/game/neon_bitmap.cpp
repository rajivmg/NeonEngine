#include "neon_bitmap.h"

#include "neon_renderer.h"

void LoadBitmap(bitmap *Bitmap, char const *Filename)
{
	file_content File = Platform.ReadFile(Filename);
	assert(File.NoError);

	tga_header *TGAHeader = (tga_header *)File.Content;
	
	// Check if the tga file type is what we want.
	// 2 == Uncompressed, True-color Image
	assert(TGAHeader->ImageType == 2);

	// Assign members with width and height values from file
	Bitmap->Width = (u32)TGAHeader->Width;
	Bitmap->Height = (u32)TGAHeader->Height;

	// Check image origin, 5th bit - 0 = lower left, 1 = upper left
	// http://www.paulbourke.net/dataformats/tga/
	if((TGAHeader->ImageDescriptor & (1 << 5)) == (1 << 5))
	{
		Bitmap->FlippedAroundY = false;
	}
	else
	{
		Bitmap->FlippedAroundY = true;
	}

	// Read bit/pixel of file
	u8 BitsPerPixel = TGAHeader->PixelDepth;

	// 4 bytes per pixel (RGBA)
	assert(BitsPerPixel == 32);

	Bitmap->BytesPerPixel = BitsPerPixel / 8;

	// Set content size
	Bitmap->DataSize = Bitmap->Width * Bitmap->Height * (BitsPerPixel / 8);

	// Allocate memory for pixel data
	Bitmap->Data = malloc(Bitmap->DataSize);

	// Copy pixel data from file to our memory
	memcpy(Bitmap->Data, (u8 *)File.Content + sizeof(tga_header), Bitmap->DataSize);

	// GL			: RRGGBBAA == 0xAABBGGRR
	// TGA ORDER	: AARRGGBB == 0xBBGGRRAA
	u32 *PixelPointer = (u32 *)Bitmap->Data;
	for(u32 PixelOffset = 0; PixelOffset < (Bitmap->DataSize / 4); ++PixelOffset)
	{
		u32 *Pixel = (PixelPointer + PixelOffset);
		u32 B = (*Pixel & 0x000000FF) << 16;
		u32 G = (*Pixel & 0x0000FF00);
		u32 R = (*Pixel & 0x00FF0000) >> 16;
		u32 A = (*Pixel & 0xFF000000);
		*Pixel = R | G | B | A;
	}

	Platform.FreeFileContent(&File);

	// Other way
	/*
	file_content File = Platform.ReadFile(Filename);
	assert(File.NoError);
	
	u8 *InBytes = (u8 *)File.Content;
	
	u8 TGAType = *(u8 *)(InBytes + 2);

	// Check if the tga file type is what we want.
	// 2 == Uncompressed, True-color Image
	assert(TGAType == 2);

	// Assign members with width and height values from file
	Bitmap->Width = *(u16 *)(InBytes + 12);
	Bitmap->Height = *(u16 *)(InBytes + 14);

	// Read bit/pixel of file
	u8 BitsPerPixel = *(u8 *)(InBytes + 16);

	// 4 bytes per pixel (RGBA)
	assert(BitsPerPixel == 32);

	Bitmap->BytesPerPixel = BitsPerPixel / 8;

	// Set content size
	Bitmap->DataSize = Bitmap->Width * Bitmap->Height * (BitsPerPixel / 8);

	// Allocate memory for pixel data
	Bitmap->Data = malloc(Bitmap->DataSize);

	// Copy pixel data from file to our memory
	memcpy(Bitmap->Data, InBytes + sizeof(tga_header), Bitmap->DataSize);

	// GL			: RRGGBBAA == 0xAABBGGRR
	// TGA ORDER	: AARRGGBB == 0xBBGGRRAA
	u32 *PixelPointer = (u32 *)Bitmap->Data;
	for(u32 PixelOffset = 0; PixelOffset < (Bitmap->DataSize / 4); ++PixelOffset)
	{
		u32 *Pixel = (PixelPointer + PixelOffset);
		u32 B = (*Pixel & 0x000000FF) << 16;
		u32 G = (*Pixel & 0x0000FF00);
		u32 R = (*Pixel & 0x00FF0000) >> 16;
		u32 A = (*Pixel & 0xFF000000);
		*Pixel = R | G | B | A;
	}

	Platform.FreeFileContent(&File);

	Bitmap->FlippedAroundY = false;
	*/
}

void FreeBitmap(bitmap *Bitmap)
{
	SAFE_FREE(Bitmap->Data);
	Bitmap->DataSize = 0;
}

void BitmapFlipAroundY(bitmap *Bitmap)
{
	Bitmap->FlippedAroundY = !Bitmap->FlippedAroundY;

	u32 *Data = (u32 *)malloc(Bitmap->DataSize);

	memcpy(Data, Bitmap->Data, Bitmap->DataSize);
	for(u32 Row = 0; Row < Bitmap->Height; ++Row)
	{
		for(u32 Col = 0; Col < Bitmap->Width; ++Col)
		{
			*((u32 *)(Bitmap->Data)+((Bitmap->Width) * (Bitmap->Height - 1 - Row)) + Col)
				= *(Data + (Bitmap->Width)* Row + Col);
		}
	}

	SAFE_FREE(Data);
}

void DebugTextureSave_(char const * Filename, bitmap *Bitmap)
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
	Header.Width = (u16)Bitmap->Width;          
	Header.Height = (u16)Bitmap->Height;         
	Header.PixelDepth = 32;     
	Header.ImageDescriptor = 0x20;
	
	if(!Bitmap->FlippedAroundY)
	{
		BitmapFlipAroundY(Bitmap);
	}

	// Change the colors byte position
	// Little-endian
	// Spec:   AA RR GG BB [ BB GG RR AA (On file) ]
	u32 *PixelPointer = (u32 *)malloc(Bitmap->DataSize);
	
	memcpy(PixelPointer, Bitmap->Data, Bitmap->DataSize);

	for(u32 PixelOffset = 0; PixelOffset < (Bitmap->DataSize / 4); ++PixelOffset)
	{
		u32 *Pixel = (PixelPointer + PixelOffset);
	
		u32 R = (*Pixel & 0x000000FF) <<16;
		u32 G = (*Pixel & 0x0000FF00);
		u32 B = (*Pixel & 0x00FF0000) >> 16;
		u32 A = (*Pixel & 0xFF000000);
		*Pixel = A | R | G | B;
	}

	void *FileContent = malloc(sizeof(tga_header) + Bitmap->DataSize);
	memcpy(FileContent, &Header, sizeof(tga_header));
	memcpy((u8 *)FileContent + sizeof(tga_header), PixelPointer, Bitmap->DataSize);
	Platform.WriteFile(Filename, sizeof(tga_header) + Bitmap->DataSize, FileContent);

	free(FileContent);
}

void InitBitmapPack(bitmap_pack *BitmapPack, u32 Width, u32 Height, u32 Padding)
{
	BitmapPack->Bitmap = {};
	BitmapPack->Bitmap.Width = Width;
	BitmapPack->Bitmap.Height = Height;
	BitmapPack->Bitmap.BytesPerPixel = 4;
	BitmapPack->Bitmap.FlippedAroundY = false;
	BitmapPack->Bitmap.DataSize = Width * Height * BitmapPack->Bitmap.BytesPerPixel;

	BitmapPack->Padding = Padding;

	BitmapPack->Bitmap.Data = malloc(BitmapPack->Bitmap.DataSize);
	if(BitmapPack->Bitmap.Data == 0)
	{
		assert(!"Malloc error");
	}

	//Texture.ContentSize = Texture.Width * Texture.Height * 4;

	memset(BitmapPack->Bitmap.Data, 0, BitmapPack->Bitmap.DataSize);

	BitmapPack->Node.Child[0] = 0;
	BitmapPack->Node.Child[1] = 0;
	BitmapPack->Node.Rect.OriginX = 1;
	BitmapPack->Node.Rect.OriginY = 1;
	BitmapPack->Node.Rect.Width = Width;
	BitmapPack->Node.Rect.Height = Height;
	BitmapPack->Node.Filled = false;

#if 0
	// make all pixels pink for debugging 
	u32 *Pixel = (u32 *)Content;	
	for(u32 x = 0; x < Width * Height; ++x)
	{
		*Pixel++ = 0xFFFF00FF; 
	}
#endif
}

static inline binary_t_node* Atlas_Insert(binary_t_node *Node, bitmap *Bitmap, u32 Padding)
{
	// if we're not a leaf node
	if(Node->Child[0] != 0 && Node->Child[1] != 0)
	{
		binary_t_node *NewNode;

		// try inserting into first child
		NewNode = Atlas_Insert(Node->Child[0], Bitmap, Padding);

		// if new node is not null
		if(NewNode != 0)
		{
			return NewNode;
		}
		else
		{
			// no room in first child, try in second child
			return Atlas_Insert(Node->Child[1], Bitmap, Padding);
		}
	}
	// if we're a leaf node
	else
	{
		// if there's already a texture here, or the we're too small for the texture
		if((Node->Filled) || (Node->Rect.Width < Bitmap->Width + Padding) || 
			(Node->Rect.Height < Bitmap->Height + Padding))
		{

			return 0;
		}

		// if we're just right size
		if((Node->Rect.Width == Bitmap->Width + Padding) && 
			(Node->Rect.Height == Bitmap->Height + Padding))
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
		u32 dw = Node->Rect.Width  - (Bitmap->Width + Padding);
		u32 dh = Node->Rect.Height - (Bitmap->Height + Padding);

		if(dw > dh)
		{
			// divide vertically
			Node->Child[0]->Rect.OriginX = Node->Rect.OriginX;
			Node->Child[0]->Rect.OriginY = Node->Rect.OriginY;
			Node->Child[0]->Rect.Width	 = (Bitmap->Width + Padding);
			Node->Child[0]->Rect.Height  = Node->Rect.Height;

			Node->Child[1]->Rect.OriginX = Node->Rect.OriginX + (Bitmap->Width + Padding);
			Node->Child[1]->Rect.OriginY = Node->Rect.OriginY;
			Node->Child[1]->Rect.Width	 = Node->Rect.Width - (Bitmap->Width + Padding);
			Node->Child[1]->Rect.Height  = Node->Rect.Height;

			return Atlas_Insert(Node->Child[0], Bitmap, Padding);
		}
		else
		{
			// divide horizontally
			Node->Child[0]->Rect.OriginX = Node->Rect.OriginX;
			Node->Child[0]->Rect.OriginY = Node->Rect.OriginY;
			Node->Child[0]->Rect.Width	 = (Bitmap->Width + Padding);
			Node->Child[0]->Rect.Height  = (Bitmap->Height + Padding);

			Node->Child[1]->Rect.OriginX = Node->Rect.OriginX;
			Node->Child[1]->Rect.OriginY = Node->Rect.OriginY + (Bitmap->Height + Padding);
			Node->Child[1]->Rect.Width	 = Node->Rect.Width;
			Node->Child[1]->Rect.Height  = Node->Rect.Height - (Bitmap->Height + Padding);

			return Atlas_Insert(Node->Child[0], Bitmap, Padding);
		}
	}
}

texture_coords BitmapPackInsert(bitmap_pack *BitmapPack, bitmap *Bitmap)
{
	binary_t_node *NodeSlot = Atlas_Insert(&BitmapPack->Node, Bitmap, BitmapPack->Padding);

	if(NodeSlot == 0)
	{
		assert(!"Null returned.");
	}

	texture_coords TCoord;

	//TCoord.BL_X = (r32)(NodeSlot->Rect.OriginX - 1.5) / (r32)(Width - 1);
	//TCoord.BL_Y = (r32)(NodeSlot->Rect.OriginY + NodeSlot->Rect.Height - Padding - 1.5) / (r32)(Height - 1);
	//TCoord.TR_X = (r32)(NodeSlot->Rect.OriginX + NodeSlot->Rect.Width - Padding - 1.5) / (r32)(Width - 1);
	//TCoord.TR_Y = (r32)(NodeSlot->Rect.OriginY - 1.5) / (r32)(Height - 1);

	TCoord.LowerLeft.x	= (r32)(NodeSlot->Rect.OriginX - 1.0) / (r32)(BitmapPack->Bitmap.Width - 0);
	TCoord.LowerLeft.y	= 1.0f - (r32)(NodeSlot->Rect.OriginY + NodeSlot->Rect.Height - BitmapPack->Padding - 1.0) / (r32)(BitmapPack->Bitmap.Height - 0);
	TCoord.UpperRight.x	= (r32)(NodeSlot->Rect.OriginX + NodeSlot->Rect.Width - BitmapPack->Padding - 1.0) / (r32)(BitmapPack->Bitmap.Width - 0);
	TCoord.UpperRight.y = 1.0f - (r32)(NodeSlot->Rect.OriginY - 1.0) / (r32)(BitmapPack->Bitmap.Height - 0);


	// Platform->Log(INFO, "BL_x=%f BL_y=%f TR_x=%f TR_y=%f\n", TCoord.BL_X, TCoord.BL_Y, TCoord.TR_X, TCoord.TR_Y);

	// Copy the texture on the atlas at its position.
	// x and y in range [1, width and height] that
	// needs to be written in range [0, Width - 1 or height - 1]
	// write_at(x, y) = (u32)content_ptr + (x-1) + ((y-1) * width)
	u32 *pTextureContent = (u32 *)Bitmap->Data;

	for(u32 y = NodeSlot->Rect.OriginY; y < NodeSlot->Rect.OriginY + NodeSlot->Rect.Height - BitmapPack->Padding; ++y)
	{
		for(u32 x = NodeSlot->Rect.OriginX; x < NodeSlot->Rect.OriginX + NodeSlot->Rect.Width - BitmapPack->Padding; ++x)
		{
			u32 *Pixel = (u32 *)BitmapPack->Bitmap.Data + (x - 1) + ((y-1) * BitmapPack->Bitmap.Width);
			*Pixel = *(pTextureContent);
			pTextureContent++;
		}
	}

	//this->GenTexture();
	//DebugTextureSave("FontAtlas.tga", &Texture);

	return TCoord;
}
