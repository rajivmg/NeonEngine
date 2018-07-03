#include "neon_font.h"

#include <rapidxml/rapidxml.hpp>
#include "neon_renderer.h"

using namespace rapidxml;

font::font(const char *FontFile)
{
    file_content FileData = Platform.ReadFile(FontFile);
    ASSERT(FileData.NoError);
    
    // Null terminate xml file for parsing
    char *XmlFile = (char *)MALLOC(sizeof(char) * (FileData.Size + 1));
    memcpy(XmlFile, FileData.Content, FileData.Size + 1);
    XmlFile[FileData.Size] = '\0';

    Platform.FreeFileContent(&FileData);

    // Parse
    xml_document<> Doc;
    Doc.parse<0>(XmlFile);

    xml_node<> *FontNode = Doc.first_node("font");
    ASSERT(FontNode);

    xml_node<> *CommonNode = FontNode->first_node("common");
    ASSERT(CommonNode);
    xml_attribute<> *LineHeightAttr = CommonNode->first_attribute("lineHeight");
    LineHeight = strtoul(LineHeightAttr->value(), nullptr, 10);
    xml_attribute<> *BaseAttr = CommonNode->first_attribute("base");
    Base = strtoul(BaseAttr->value(), nullptr, 10);
    xml_attribute<> *ScaleWAttr = CommonNode->first_attribute("scaleW");
    ScaleW = strtoul(ScaleWAttr->value(), nullptr, 10);
    xml_attribute<> *ScaleHAttr = CommonNode->first_attribute("scaleH");
    ScaleH = strtoul(ScaleHAttr->value(), nullptr, 10);
    xml_attribute<> *PagesAttr = CommonNode->first_attribute("pages");
    u32 Pages = strtoul(PagesAttr->value(), nullptr, 10);
    ASSERT(Pages == 1);

    xml_node<> *PagesNode = FontNode->first_node("pages");
    xml_node<> *Page0Node = PagesNode->first_node("page");
    xml_attribute<> *Page0IDAttr = Page0Node->first_attribute("id");
    u32 Page0ID = strtoul(Page0IDAttr->value(), nullptr, 10);
    ASSERT(Page0ID == 0);
    xml_attribute<> *Page0FileAttr = Page0Node->first_attribute("file");
    bitmap Page0Bitmap;
    char Page0FilePath[1024] = "fonts/";
    strncat(Page0FilePath, Page0FileAttr->value(), 1023-6);
    //LoadBitmap(&Page0Bitmap, Page0FileAttr->value());
    LoadBitmap(&Page0Bitmap, Page0FilePath);
    Texture = rndr::MakeTexture(&Page0Bitmap, texture_type::TEXTURE_2D, texture_filter::NEAREST, texture_wrap::CLAMP, false);
    FreeBitmap(&Page0Bitmap);

    xml_node<> *CharsNode = FontNode->first_node("chars");
    xml_attribute<> *CharsCountAttr = CharsNode->first_attribute("count");
    GlyphsCount = strtoul(CharsCountAttr->value(), nullptr, 10);
    Glyphs = MALLOC_STRUCT(glyph, GlyphsCount);
    xml_node<> *CharFirstNode = CharsNode->first_node("char");
    xml_node<> *CharNode = CharFirstNode;
    u32 GlyphIndex = 0;
    while(CharNode)
    {
        xml_attribute<> *CharIdAttr = CharNode->first_attribute("id");
        xml_attribute<> *CharXAttr = CharNode->first_attribute("x");
        xml_attribute<> *CharYAttr = CharNode->first_attribute("y");
        xml_attribute<> *CharWidthAttr = CharNode->first_attribute("width");
        xml_attribute<> *CharHeightAttr = CharNode->first_attribute("height");
        xml_attribute<> *CharXOffAttr = CharNode->first_attribute("xoffset");
        xml_attribute<> *CharYOffAttr = CharNode->first_attribute("yoffset");
        xml_attribute<> *CharXAdvanAttr = CharNode->first_attribute("xadvance");

        Glyphs[GlyphIndex].Id = strtoul(CharIdAttr->value(), nullptr, 10); // UInt
        Glyphs[GlyphIndex].Rect.x = (r32)strtoul(CharXAttr->value(), nullptr, 10); // UInt
        Glyphs[GlyphIndex].Rect.y = (r32)strtoul(CharYAttr->value(), nullptr, 10); // UInt
        Glyphs[GlyphIndex].Rect.width = (r32)strtoul(CharWidthAttr->value(), nullptr, 10); // UInt
        Glyphs[GlyphIndex].Rect.height = (r32)strtoul(CharHeightAttr->value(), nullptr, 10); // UInt
        Glyphs[GlyphIndex].XOffset = strtol(CharXOffAttr->value(), nullptr, 10); // Int
        Glyphs[GlyphIndex].YOffset = strtol(CharYOffAttr->value(), nullptr, 10); // Int
        Glyphs[GlyphIndex].XAdvance = strtol(CharXAdvanAttr->value(), nullptr, 10); // Int

        ++GlyphIndex;
        CharNode = CharNode->next_sibling("char");
    }
    ASSERT(GlyphIndex == GlyphsCount);

    Doc.clear();
    SAFE_FREE(XmlFile);
}

font::~font()
{
    Free();
}

void font::Free()
{
    SAFE_FREE(Glyphs);
}

glyph *font::GetGlyph(u32 Codepoint)
{
    glyph *Result;

    // Binary search the Glyphs array
    int Lo = 0, Hi = GlyphsCount - 1;

    while(Lo <= Hi)
    {
        int Mid = (Lo + Hi) / 2;

        if(Glyphs[Mid].Id < Codepoint)
        {
            Lo = Mid + 1;
        }
        else if(Glyphs[Mid].Id > Codepoint)
        {
            Hi = Mid - 1;
        }
        else
        {
            return &Glyphs[Mid];
        }
    }

    ASSERT(!"Codepoint not found!");
    return Result;
}

#if 0
void InitFont(font *Font, char const *FontSrcFile, u32 FontHeight)
{
    Font->Height = FontHeight;
    Font->Glyphs = (glyph *)malloc(sizeof(glyph) * (128 - 32));

    bitmap_pack BitmapPack;
    InitBitmapPack(&BitmapPack, 512, 512, 2);

    FT_Library FTLib;
    FT_Face Face;

    int Error = FT_Init_FreeType(&FTLib);
    if(Error)
    {
        ASSERT(!"Error initialising the FreeType library.");
    }

    file_content FontData = Platform.ReadFile(FontSrcFile);
    ASSERT(FontData.NoError);

    Error = FT_New_Memory_Face(FTLib, (const FT_Byte *)FontData.Content, (FT_Long)FontData.Size, 0, &Face);
    if(Error == FT_Err_Unknown_File_Format)
    {
        ASSERT(!"File format is not supported.");
    }
    else if(Error)
    {
        ASSERT(!"File could not be opened or read.");
    }

    Error = FT_Set_Pixel_Sizes(Face, 0, FontHeight);
    ASSERT(Error == 0);

    bitmap *GlyphTexture = new bitmap();
    for(int CIndex = 32; CIndex < 128; ++CIndex)
    {
        int GlyphIndex = FT_Get_Char_Index(Face, CIndex);

        Error = FT_Load_Glyph(Face, GlyphIndex, FT_LOAD_DEFAULT);
        ASSERT(Error == 0);

        if(Face->glyph->format != FT_GLYPH_FORMAT_BITMAP)
        {
            FT_Render_Glyph(Face->glyph, FT_RENDER_MODE_NORMAL);
        }

        glyph *Glyph = Font->Glyphs + (CIndex - 32);
        Glyph->Width = Face->glyph->bitmap.width;
        Glyph->Height = Face->glyph->bitmap.rows;

        u8 *GlyphBitmap = Face->glyph->bitmap.buffer;

        FT_Glyph_Metrics *Metrics = &Face->glyph->metrics;

        Glyph->HoriBearingX = Metrics->horiBearingX / 64;
        Glyph->HoriBearingY = Metrics->horiBearingY / 64;
        Glyph->HoriAdvance = Metrics->horiAdvance / 64;
        Glyph->Hang = Glyph->HoriBearingY - Glyph->Height;

        GlyphTexture->Width = Glyph->Width;
        GlyphTexture->Height = Glyph->Height;
        GlyphTexture->DataSize = GlyphTexture->Width * GlyphTexture->Height * 4;
        if(GlyphTexture->DataSize != 0)
        {
            GlyphTexture->Data = malloc(GlyphTexture->DataSize);
            ASSERT(GlyphTexture != 0);
            memset(GlyphTexture->Data, 255, GlyphTexture->DataSize);
        }

        u32 *DestTexel = (u32 *)GlyphTexture->Data;
        u8 *SrcTexel = (u8 *)GlyphBitmap;

        // Convert Glpyh's 8bit texture to 32bit texture
        for(u32 x = 0; x < GlyphTexture->Width * GlyphTexture->Height; ++x)
        {
            u8 *DestGreen = (u8 *)DestTexel + 3;
            *DestGreen = *SrcTexel;

            DestTexel++;
            SrcTexel++;
        }

        if(GlyphTexture->DataSize != 0)
        {
            Glyph->Coords = BitmapPackInsert(&BitmapPack, GlyphTexture);
        }

        // After glyph texture has been copied to texture atlas free the glyph texture memory
        if(GlyphTexture->DataSize != 0)
        {
            SAFE_FREE(GlyphTexture->Data);
        }
    }

    Font->Texture = rndr::MakeTexture(&BitmapPack.Bitmap, texture_type::TEXTURE_2D, texture_filter::LINEAR, texture_wrap::CLAMP, false);
    FreeBitmapPack(&BitmapPack);
    Platform.FreeFileContent(&FontData);

    // DebugTextureSave("FontAtlas.tga", &Atlas.Texture);
}

void FreeFont(font *Font)
{
    SAFE_FREE(Font->Glyphs);
}

vec2 GetTextDim(font *Font, char const *Format, ...)
{
    vec2 Result(0, 0);

    char Text[8192];

    va_list Arguments;
    va_start(Arguments, Format);
    vsnprintf(Text, 8192, Format, Arguments);
    va_end(Arguments);

    vec2 Dim(0, 0);
    Dim.y = (r32)Font->Height;
    s32 MaxHang = 0;
    s32 PrevX = 0;
    int Index = 0;
    while(Text[Index] != 0)
    {
        if((int)Text[Index] == 10)
        {
            Dim.y += (r32)Font->Height;
            ++Index;
            MaxHang = 0;
            PrevX = (s32)Dim.x;
            Dim.x = 0;
            continue;
        }

        glyph *CharGlyph = Font->Glyphs + ((int)Text[Index] - 32);
        Dim.x += CharGlyph->HoriAdvance;
        ++Index;

        if(CharGlyph->Hang * -1 > MaxHang)
        {
            MaxHang = CharGlyph->Hang * -1;
        }
    }


    Result.x = (Dim.x >= PrevX ? Dim.x : PrevX);
    Result.y = (Dim.y + MaxHang);
    return Result;
}
#endif