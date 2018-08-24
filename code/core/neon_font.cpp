#include "neon_font.h"

#include "neon_renderer.h"

#include <rapidxml/rapidxml.hpp>
using namespace rapidxml;

void InitFont(font *Font, const char *FontFile)
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
    Font->LineHeight = strtoul(LineHeightAttr->value(), nullptr, 10);
    xml_attribute<> *BaseAttr = CommonNode->first_attribute("base");
    Font->Base = strtoul(BaseAttr->value(), nullptr, 10);
    xml_attribute<> *ScaleWAttr = CommonNode->first_attribute("scaleW");
    Font->ScaleW = strtoul(ScaleWAttr->value(), nullptr, 10);
    xml_attribute<> *ScaleHAttr = CommonNode->first_attribute("scaleH");
    Font->ScaleH = strtoul(ScaleHAttr->value(), nullptr, 10);
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
    Font->Texture = rndr::MakeTexture(&Page0Bitmap, tex_param::TEX2D, tex_param::NEAREST, tex_param::CLAMP, false);
    FreeBitmap(&Page0Bitmap);

    xml_node<> *CharsNode = FontNode->first_node("chars");
    xml_attribute<> *CharsCountAttr = CharsNode->first_attribute("count");
    Font->GlyphsCount = strtoul(CharsCountAttr->value(), nullptr, 10);
    Font->Glyphs = MALLOC_STRUCT(glyph, Font->GlyphsCount);
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

        Font->Glyphs[GlyphIndex].Id = strtoul(CharIdAttr->value(), nullptr, 10); // UInt
        Font->Glyphs[GlyphIndex].Rect.x = (r32)strtoul(CharXAttr->value(), nullptr, 10); // UInt
        Font->Glyphs[GlyphIndex].Rect.y = (r32)strtoul(CharYAttr->value(), nullptr, 10); // UInt
        Font->Glyphs[GlyphIndex].Rect.width = (r32)strtoul(CharWidthAttr->value(), nullptr, 10); // UInt
        Font->Glyphs[GlyphIndex].Rect.height = (r32)strtoul(CharHeightAttr->value(), nullptr, 10); // UInt
        Font->Glyphs[GlyphIndex].XOffset = strtol(CharXOffAttr->value(), nullptr, 10); // Int
        Font->Glyphs[GlyphIndex].YOffset = strtol(CharYOffAttr->value(), nullptr, 10); // Int
        Font->Glyphs[GlyphIndex].XAdvance = strtol(CharXAdvanAttr->value(), nullptr, 10); // Int

        ++GlyphIndex;
        CharNode = CharNode->next_sibling("char");
    }
    ASSERT(GlyphIndex == Font->GlyphsCount);

    Doc.clear();
    SAFE_FREE(XmlFile);
}

void FreeFont(font *Font)
{
    SAFE_FREE(Font->Glyphs);
}

glyph *FontGetGlyph(font *Font, u32 Codepoint)
{
    // Binary search the Glyphs array
    int Lo = 0, Hi = Font->GlyphsCount - 1;

    while(Lo <= Hi)
    {
        int Mid = (Lo + Hi) / 2;

        if(Font->Glyphs[Mid].Id < Codepoint)
        {
            Lo = Mid + 1;
        }
        else if(Font->Glyphs[Mid].Id > Codepoint)
        {
            Hi = Mid - 1;
        }
        else
        {
            return &Font->Glyphs[Mid];
        }
    }

    ASSERT(!"Codepoint not found!");
    return nullptr;
}

#if 0
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