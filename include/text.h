#pragma once


#include <misc.h>

void CM_LoadFont(const char* path, int GFXwidth, int GFXheight, const void* bitmap);

void CM_DrawText(int Screen, const char* text, const std::string& fontName, int color, Pos position);
