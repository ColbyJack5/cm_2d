#include <stdio.h>
#include <nds.h>
#include <nf_lib.h>
#include <cm_2d.h>
#include "take2_0.h"


int main(int argc, char **argv) {
	Initialize(); //This sets up everything, call it at the beginning of every program

	//Create a palette and load it onto both screens
	CM_CreateSpritePalette("sprites/extended_palette");
	CM_LoadSpritePaletteToVRAM(ScreenConstants::BOTTOMSCREEN, "sprites/extended_palette");
	CM_LoadSpritePaletteToVRAM(ScreenConstants::TOPSCREEN, "sprites/extended_palette");

	//Define an initial position for the sprites
	Pos spritePosition = Pos(ScreenConstants::SCREENWIDTH / 2 - SpriteConstants::SPRITESIZE32 / 2, ScreenConstants::SCREENHEIGHT / 2 - SpriteConstants::SPRITESIZE32 / 2);

	//Create a template for the "man" sprite and load it onto a screen
	CM_CreateSpriteTemplate("man", SpriteConstants::SPRITESIZE32, SpriteConstants::SPRITESIZE32, true);
	CM_LoadSpriteTemplateToVRAM(ScreenConstants::BOTTOMSCREEN, "man");

	//Add Animations to the "man" template
	CM_AddAnimationToTemplate("man", "walk_up",		{ 0,1,2,1 }, 3);
	CM_AddAnimationToTemplate("man", "walk_right",	{ 3,4,5,4 }, 3);
	CM_AddAnimationToTemplate("man", "walk_down",	{ 6,7,8,7 }, 3);
	CM_AddAnimationToTemplate("man", "walk_left",	{ 9,10,11,10 }, 3);

	//Create a template for the "woman" sprite and load it onto a screen
	CM_CreateSpriteTemplate("woman", SpriteConstants::SPRITESIZE32, SpriteConstants::SPRITESIZE32, true);
	CM_LoadSpriteTemplateToVRAM(ScreenConstants::TOPSCREEN, "woman");

	CM_AddAnimationToTemplate("woman", "walk_up", { 0,1,2,1 }, 3);
	CM_AddAnimationToTemplate("woman", "walk_right", { 3,4,5,4 }, 3);
	CM_AddAnimationToTemplate("woman", "walk_down", { 6,7,8,7 }, 3);
	CM_AddAnimationToTemplate("woman", "walk_left", { 9,10,11,10 }, 3);
	
	//Create an instance of the "man" sprite
	Identifier manSprite = CM_CreateSprite(ScreenConstants::BOTTOMSCREEN, "man", spritePosition);

	//Create an instance of the "woman" sprite
	Identifier womanSprite = CM_CreateSprite(ScreenConstants::TOPSCREEN, "woman", spritePosition);

	Pos movementLastFrame = Pos(0, 0);
	while (!(keysDown() & KEY_START)) { //End the game when the start key is pressed
		scanKeys();
		Pos movement = Pos(0, 0);
		if (keysHeld() & KEY_UP) movement.y--;
		if (keysHeld() & KEY_DOWN) movement.y++;
		if (keysHeld() & KEY_LEFT) movement.x--;
		if (keysHeld() & KEY_RIGHT) movement.x++;
		if (movementLastFrame != movement) {
			if (movement.y < 0) {
				CM_SetSpriteAnimation(manSprite, "walk_up");
				CM_SetSpriteAnimation(womanSprite, "walk_up");
			}
			if (movement.y > 0) {
				CM_SetSpriteAnimation(manSprite, "walk_down");
				CM_SetSpriteAnimation(womanSprite, "walk_down");
			}
			if (movement.x < 0) {
				CM_SetSpriteAnimation(manSprite, "walk_left");
				CM_SetSpriteAnimation(womanSprite, "walk_left");
			}
			if (movement.x > 0) {
				CM_SetSpriteAnimation(manSprite, "walk_right");
				CM_SetSpriteAnimation(womanSprite, "walk_right");
			}
			if (movement == Pos(0, 0)) {
				CM_PauseSpriteAnimation(manSprite);
				CM_PauseSpriteAnimation(womanSprite);
			}
		}
		spritePosition += movement;
		movementLastFrame = movement;
		CM_MoveSprite(manSprite, spritePosition);
		CM_MoveSprite(womanSprite, spritePosition);
		UpdateScreen();
	}
	ClearScreen();
	return 0;

}