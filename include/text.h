#pragma once


#include <misc.h>

void CM_LoadFont(const char* path, int GFXwidth, int GFXheight, int textureFormat, const void* bitmap);

void CM_DrawText(int Screen,char* text, const std::string& fontName, Pos position);
