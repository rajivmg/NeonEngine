#ifndef F_EDITOR_H
#define F_EDITOR_H

#include <core/neon_platform.h>
#include <unordered_map>

struct ed_tilesets
{
    u32 ID;
    char File[128];
};

struct editor_state
{
    //std::unordered_map<u32, std::string> Tilesets;
    ed_tilesets Tilesets[16];
    u32 TilesetsCount;

    // Internal
    char *TilesetsXmlFile;
};

void InitEditor(editor_state *State);
void Editor(editor_state *State);
#endif