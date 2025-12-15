#pragma once


#include <misc.h>

void CM_LoadFont(const void* fontData, int fontDataSize);

void CM_LoadFont(const char* path);

void CM_DrawText(int Scrren, char* text, Pos position);
