#include <stdio.h>
#include <nds.h>
#include <nf_lib.h>
#include <cm_2d.h>
#include "blocky_0.h"
#include "take2_0.h"
#include "Mario_0.h"

int main(int argc, char **argv) {
	Initialize(); //This sets up everything, call it at the beginning of every program
	

	CM_LoadFont("fonts/take2.fnt", 128, 128, take2_0Bitmap); 

	// Add Colors To Palette (used for text)
	NF_8BITS_BACKBUFFER[ScreenConstants::BOTTOMSCREEN].pal[1] = RGB15(31, 31, 31); //White  
	NF_8BITS_BACKBUFFER[ScreenConstants::TOPSCREEN].pal[1] = RGB15(31, 31, 31); //White
  
	//Load a backgroun
	NF_LoadTiledBg("backgrounds/blue", "blue", 256, 256);

	//Create two instances of the "blue" background
	NF_CreateTiledBg(ScreenConstants::BOTTOMSCREEN, ScreenConstants::LAYER0, "blue");
	NF_CreateTiledBg(ScreenConstants::TOPSCREEN,	ScreenConstants::LAYER0, "blue");

	//Create a palette and load it onto both screens
	CM_CreateSpritePalette("sprites/extended_palette");
	CM_LoadSpritePaletteToVRAM(ScreenConstants::BOTTOMSCREEN, "sprites/extended_palette");
	CM_LoadSpritePaletteToVRAM(ScreenConstants::TOPSCREEN, "sprites/extended_palette");

	//Define an initial position and velocity for the ball
	Pos ballPosition = Pos(ScreenConstants::SCREENWIDTH / 2 - SpriteConstants::SPRITESIZE16 / 2, ScreenConstants::SCREENHEIGHT / 2 - SpriteConstants::SPRITESIZE16 / 2);
	Pos ballVelocity = Pos(2, 2);

	//Create a template for the "ball" sprite and load it onto a screen
	CM_CreateSpriteTemplate("ball", SpriteConstants::SPRITESIZE16, SpriteConstants::SPRITESIZE16, true);
	CM_LoadSpriteTemplateToVRAM(ScreenConstants::BOTTOMSCREEN, "ball");

	//Create an instance of the "ball" sprite
	Identifier ballSprite = CM_CreateSprite(ScreenConstants::BOTTOMSCREEN, "ball", ballPosition);

	//Define an initial position and vertical speed for the player paddles
	Pos bar1position = Pos(10, 20);
	Pos bar2position = Pos(230, 20);
	int barSpeed = 3;

	//Create a template for the "bar" sprite and load it onto a screen
	CM_CreateSpriteTemplate("bar", SpriteConstants::SPRITESIZE16, SpriteConstants::SPRITESIZE32, true);
	CM_LoadSpriteTemplateToVRAM(ScreenConstants::BOTTOMSCREEN, "bar");

	//Create twp instances of the "bar" sprite
	Identifier player1bar = CM_CreateSprite(ScreenConstants::BOTTOMSCREEN, "bar", bar1position);
	Identifier player2bar = CM_CreateSprite(ScreenConstants::BOTTOMSCREEN, "bar", bar2position);

	int player1score = 0;
	int player2score = 0;
	while (!(keysDown() & KEY_START)) { //End the game when the start key is pressed
		scanKeys();
		//Handle player input
		if (keysHeld() & KEY_UP && bar1position.y - barSpeed >= 0) {
			bar1position.y -= barSpeed;
		}
		if (keysHeld() & KEY_DOWN && bar1position.y + barSpeed <= ScreenConstants::SCREENHEIGHT - SpriteConstants::SPRITESIZE32) {
			bar1position.y += barSpeed;
		}
		if (keysHeld() & KEY_X && bar2position.y - barSpeed >= 0) {
			bar2position.y -= barSpeed;
		}
		if (keysHeld() & KEY_B && bar2position.y + barSpeed <= ScreenConstants::SCREENHEIGHT - SpriteConstants::SPRITESIZE32) {
			bar2position.y += barSpeed;
		}

		ballPosition += ballVelocity;
		if (ballPosition.y <= 0 || ballPosition.y >= ScreenConstants::SCREENHEIGHT - SpriteConstants::SPRITESIZE16) { //Bounce off the floor and ceiling 
			ballVelocity *= Pos(1, -1);
		}
		if(ballPosition.x <=0) { //Bounce off the left wall
			ballVelocity *= Pos(-1, 1);
			player2score++;
		}
		if (ballPosition.x >= ScreenConstants::SCREENWIDTH - SpriteConstants::SPRITESIZE16) { //Bounce off the right wall
			ballVelocity *= Pos(-1, 1);
			player1score++;
		}
		if (ballPosition.x <= bar1position.x + SpriteConstants::SPRITESIZE16 - 2 && ballPosition.x >= bar1position.x + 2) {
			if (ballPosition.y == bar1position.y + SpriteConstants::SPRITESIZE32 || ballPosition.y + SpriteConstants::SPRITESIZE16 == bar1position.y) {
				ballVelocity *= Pos(1, -1);
			}
			if (ballPosition.y < bar1position.y + SpriteConstants::SPRITESIZE32 && ballPosition.y + SpriteConstants::SPRITESIZE16 > bar1position.y) {
				ballVelocity *= Pos(-1, 1);
			}
		}
		if (ballPosition.x + SpriteConstants::SPRITESIZE16 >= bar2position.x + 2 && ballPosition.x <= bar2position.x + 2) {
			if (ballPosition.y == bar2position.y + SpriteConstants::SPRITESIZE32 || ballPosition.y + SpriteConstants::SPRITESIZE16 == bar2position.y) {
				ballVelocity *= Pos(1, -1);
			}
			if (ballPosition.y < bar2position.y + SpriteConstants::SPRITESIZE32 && ballPosition.y + SpriteConstants::SPRITESIZE16 > bar2position.y) {
				ballVelocity *= Pos(-1, 1);
			}
		}



		//Place player scores on top screen
		CM_DrawText(ScreenConstants::TOPSCREEN, ("Player 1: " + std::to_string(player1score)).c_str(), "take2", 1, Pos(10, 10));
		CM_DrawText(ScreenConstants::TOPSCREEN, ("Player 2: " + std::to_string(player2score)).c_str(), "take2", 1, Pos(200, 182));

		//Update the visuals
		CM_MoveSprite(ballSprite, ballPosition);
		CM_MoveSprite(player1bar, bar1position);
		CM_MoveSprite(player2bar, bar2position);
		UpdateScreen();
	}
	ClearScreen();
	return 0;

}