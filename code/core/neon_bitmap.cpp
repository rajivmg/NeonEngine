#include "neon_bitmap.h"

#include "neon_renderer.h"

void LoadBitmap(bitmap *Bitmap, char const *Filename)
{
    file_content File = Platform.ReadFile(Filename);
    ASSERT(File.NoError);

    tga_header *TGAHeader = (tga_header *)File.Content;

    // Check if the tga file type is what we want.
    // 2 == Uncompressed, True-color Image
    ASSERT(TGAHeader->ImageType == 2);

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
    ASSERT(BitsPerPixel == 32);

    Bitmap->BytesPerPixel = BitsPerPixel / 8;

    // Set content size
    Bitmap->DataSize = Bitmap->Width * Bitmap->Height * (BitsPerPixel / 8);

    // Allocate memory for pixel data
    Bitmap->Data = MALLOC(Bitmap->DataSize);

    // Copy pixel data from file to our memory
    memcpy(Bitmap->Data, (u8 *)File.Content + sizeof(tga_header), Bitmap->DataSize);

    // GL           : RRGGBBAA == 0xAABBGGRR
    // TGA ORDER    : AARRGGBB == 0xBBGGRRAA
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

    // GL           : RRGGBBAA == 0xAABBGGRR
    // TGA ORDER    : AARRGGBB == 0xBBGGRRAA
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

void LoadBitmap(bitmap *Bitmap, file_content File)
{
    ASSERT(File.NoError);

    tga_header *TGAHeader = (tga_header *)File.Content;

    // Check if the tga file type is what we want.
    // 2 == Uncompressed, True-color Image
    ASSERT(TGAHeader->ImageType == 2);

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
    ASSERT(BitsPerPixel == 32);

    Bitmap->BytesPerPixel = BitsPerPixel / 8;

    // Set content size
    Bitmap->DataSize = Bitmap->Width * Bitmap->Height * (BitsPerPixel / 8);

    // Allocate memory for pixel data
    Bitmap->Data = MALLOC(Bitmap->DataSize);

    // Copy pixel data from file to our memory
    memcpy(Bitmap->Data, (u8 *)File.Content + sizeof(tga_header), Bitmap->DataSize);

    // GL           : RRGGBBAA == 0xAABBGGRR
    // TGA ORDER    : AARRGGBB == 0xBBGGRRAA
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
}

void FreeBitmap(bitmap *Bitmap)
{
    SAFE_FREE(Bitmap->Data);
    Bitmap->DataSize = 0;
}

void BitmapFlipAroundY(bitmap *Bitmap)
{
    Bitmap->FlippedAroundY = !Bitmap->FlippedAroundY;

    u32 *Data = (u32 *)MALLOC(Bitmap->DataSize);

    memcpy(Data, Bitmap->Data, Bitmap->DataSize);
    for(u32 Row = 0; Row < Bitmap->Height; ++Row)
    {
        for(u32 Col = 0; Col < Bitmap->Width; ++Col)
        {
            *((u32 *)(Bitmap->Data) + ((Bitmap->Width) * (Bitmap->Height - 1 - Row)) + Col)
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
    u32 *PixelPointer = (u32 *)MALLOC(Bitmap->DataSize);

    memcpy(PixelPointer, Bitmap->Data, Bitmap->DataSize);

    for(u32 PixelOffset = 0; PixelOffset < (Bitmap->DataSize / 4); ++PixelOffset)
    {
        u32 *Pixel = (PixelPointer + PixelOffset);

        u32 R = (*Pixel & 0x000000FF) << 16;
        u32 G = (*Pixel & 0x0000FF00);
        u32 B = (*Pixel & 0x00FF0000) >> 16;
        u32 A = (*Pixel & 0xFF000000);
        *Pixel = A | R | G | B;
    }

    void *FileContent = MALLOC(sizeof(tga_header) + Bitmap->DataSize);
    memcpy(FileContent, &Header, sizeof(tga_header));
    memcpy((u8 *)FileContent + sizeof(tga_header), PixelPointer, Bitmap->DataSize);
    Platform.WriteFile(Filename, sizeof(tga_header) + Bitmap->DataSize, FileContent);

    SAFE_FREE(FileContent);
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

    BitmapPack->Bitmap.Data = MALLOC(BitmapPack->Bitmap.DataSize);
    if(BitmapPack->Bitmap.Data == 0)
    {
        ASSERT(!"Malloc error");
    }

    memset(BitmapPack->Bitmap.Data, 0, BitmapPack->Bitmap.DataSize);

    BitmapPack->Node.Child[0] = 0;
    BitmapPack->Node.Child[1] = 0;
    BitmapPack->Node.Rect.x = 1;
    BitmapPack->Node.Rect.y = 1;
    BitmapPack->Node.Rect.width = (r32)Width;
    BitmapPack->Node.Rect.height = (r32)Height;
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

void FreeBitmapPack(bitmap_pack *BitmapPack)
{
    FreeBitmap(&BitmapPack->Bitmap);
    // NOTE: Not free-ing binary_t_node Nodes.
}

static binary_t_node* RectPack(binary_t_node *Node, bitmap *Bitmap, u32 Padding)
{
    // if we're not a leaf node
    if(Node->Child[0] != 0 && Node->Child[1] != 0)
    {
        binary_t_node *NewNode;

        // try inserting into first child
        NewNode = RectPack(Node->Child[0], Bitmap, Padding);

        // if new node is not null
        if(NewNode != 0)
        {
            return NewNode;
        }
        else
        {
            // no room in first child, try in second child
            return RectPack(Node->Child[1], Bitmap, Padding);
        }
    }
    // if we're a leaf node
    else
    {
        // if there's already a texture here, or the we're too small for the texture
        if((Node->Filled) || (Node->Rect.width < Bitmap->Width + Padding) ||
            (Node->Rect.height < Bitmap->Height + Padding))
        {

            return 0;
        }

        // if we're just right size
        if((Node->Rect.width == Bitmap->Width + Padding) &&
            (Node->Rect.height == Bitmap->Height + Padding))
        {
            Node->Filled = true;
            return Node;
        }

        Node->Child[0] = (binary_t_node *)MALLOC(sizeof(binary_t_node));
        Node->Child[1] = (binary_t_node *)MALLOC(sizeof(binary_t_node));

        Node->Child[0]->Filled = false;
        Node->Child[1]->Filled = false;

        Node->Child[0]->Child[0] = 0;
        Node->Child[0]->Child[1] = 0;
        Node->Child[1]->Child[0] = 0;
        Node->Child[1]->Child[1] = 0;


        // decide which way to split
        r32 dw = Node->Rect.width - (Bitmap->Width + Padding);
        r32 dh = Node->Rect.height - (Bitmap->Height + Padding);

        if(dw > dh)
        {
            // divide vertically
            Node->Child[0]->Rect.x = Node->Rect.x;
            Node->Child[0]->Rect.y= Node->Rect.y;
            Node->Child[0]->Rect.width= (r32)(Bitmap->Width + Padding);
            Node->Child[0]->Rect.height = Node->Rect.height;

            Node->Child[1]->Rect.x = Node->Rect.x + (Bitmap->Width + Padding);
            Node->Child[1]->Rect.y = Node->Rect.y;
            Node->Child[1]->Rect.width = Node->Rect.width - (Bitmap->Width + Padding);
            Node->Child[1]->Rect.height = Node->Rect.height;

            return RectPack(Node->Child[0], Bitmap, Padding);
        }
        else
        {
            // divide horizontally
            Node->Child[0]->Rect.x = Node->Rect.x;
            Node->Child[0]->Rect.y = Node->Rect.y;
            Node->Child[0]->Rect.width = (r32)(Bitmap->Width + Padding);
            Node->Child[0]->Rect.height = (r32)(Bitmap->Height + Padding);

            Node->Child[1]->Rect.x = Node->Rect.x;
            Node->Child[1]->Rect.y = Node->Rect.y + (Bitmap->Height + Padding);
            Node->Child[1]->Rect.width = Node->Rect.width;
            Node->Child[1]->Rect.height = Node->Rect.height - (Bitmap->Height + Padding);

            return RectPack(Node->Child[0], Bitmap, Padding);
        }
    }
}

vec4 BitmapPackInsert(bitmap_pack *BitmapPack, bitmap *Bitmap)
{
    binary_t_node *NodeSlot = RectPack(&BitmapPack->Node, Bitmap, BitmapPack->Padding);

    if(NodeSlot == 0)
    {
        ASSERT(!"Null returned.");
    }

    vec4 TCoord;

    TCoord.x = (r32)(NodeSlot->Rect.x - 1.0) / (r32)(BitmapPack->Bitmap.Width);
    TCoord.y = 1.0f - (r32)(NodeSlot->Rect.y + NodeSlot->Rect.height - BitmapPack->Padding - 1.0) / (r32)(BitmapPack->Bitmap.Height);
    TCoord.z = (r32)(NodeSlot->Rect.x + NodeSlot->Rect.width - BitmapPack->Padding - 1.0) / (r32)(BitmapPack->Bitmap.Width);
    TCoord.w = 1.0f - (r32)(NodeSlot->Rect.y - 1.0) / (r32)(BitmapPack->Bitmap.Height);

    // Copy the texture on the atlas at its position.
    // x and y in range [1, width and height] that
    // needs to be written in range [0, Width - 1 or height - 1]
    // write_at(x, y) = (u32)content_ptr + (x-1) + ((y-1) * width)
    u32 *pTextureContent = (u32 *)Bitmap->Data;

    for(u32 y = (u32)NodeSlot->Rect.y; y < NodeSlot->Rect.y + NodeSlot->Rect.height - BitmapPack->Padding; ++y)
    {
        for(u32 x = (u32)NodeSlot->Rect.x; x < NodeSlot->Rect.x + NodeSlot->Rect.width - BitmapPack->Padding; ++x)
        {
            u32 *Pixel = (u32 *)BitmapPack->Bitmap.Data + (x - 1) + ((y - 1) * BitmapPack->Bitmap.Width);
            *Pixel = *(pTextureContent);
            pTextureContent++;
        }
    }

    //this->GenTexture();
    //DebugTextureSave("FontAtlas.tga", &Texture);

    return TCoord;
}
