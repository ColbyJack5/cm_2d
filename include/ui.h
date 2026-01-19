#pragma once
#include <string>
#include "misc.h"


void Initialize();

void UpdateScreen();

void ClearScreen();

void DrawLine(int screen, int color, Pos end1,Pos end2);

void DrawBox(int screen, int color, Pos corner, Pos opposingCorner);