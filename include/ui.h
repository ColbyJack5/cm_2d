#pragma once
#include <nds.h>
#include <string>
#include "misc.h"


void Initialize();

u32 Info(std::string infoString);

void UpdateScreen();

void ClearScreen();

void DrawLine(int screen, Pos end1,Pos end2);

void DrawBox(int screen, Pos corner, Pos opposingCorner);