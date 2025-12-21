#include <ui.h>
#include <string>
#include <filesystem.h>
#include <nf_lib.h>
#include <constants.h>
#include <text.h>
#include <sprites.h>

void wait_forever(void)
{
    while (1)
        swiWaitForVBlank();
}

void Initialize(){
	NF_Set2D(ScreenConstants::topScreen, 0);
	NF_Set2D(ScreenConstants::bottomScreen, 0);	
	consoleDebugInit(DebugDevice_NOCASH);
	swiWaitForVBlank();
	NF_SetRootFolder("NITROFS");	// Define the Root folder to use NITROFS

	NF_Set2D(ScreenConstants::topScreen, 5);				// Mode2D_5 on both screens
	NF_Set2D(ScreenConstants::bottomScreen, 5);
    bool init_ok = nitroFSInit(NULL);
    if (!init_ok)
    {
        wait_forever();
    }
    NF_InitMixedBgSys(ScreenConstants::topScreen);
	NF_InitMixedBgSys(ScreenConstants::bottomScreen);
    NF_InitTiledBgBuffers();
    NF_Init8bitsBgBuffers();
    NF_Init8bitsBackBuffer(ScreenConstants::bottomScreen);
    NF_Enable8bitsBackBuffer(ScreenConstants::bottomScreen);
	NF_Init8bitsBackBuffer(ScreenConstants::topScreen);
    NF_Enable8bitsBackBuffer(ScreenConstants::topScreen);
	NF_InitSpriteBuffers();
	NF_InitSpriteSys(ScreenConstants::topScreen);
	NF_InitSpriteSys(ScreenConstants::bottomScreen);
	//LoadFont();
	setBrightness(3, 0);
}


void UpdateScreen(){
    CM_UpdateSprites();
    NF_SpriteOamSet(ScreenConstants::topScreen);
    NF_SpriteOamSet(ScreenConstants::bottomScreen);
    swiWaitForVBlank();		
    oamUpdate(&oamMain);
    oamUpdate(&oamSub);
    NF_Flip8bitsBackBuffer(ScreenConstants::bottomScreen,1);
    free(NF_8BITS_BACKBUFFER[ScreenConstants::bottomScreen].data);
    NF_8BITS_BACKBUFFER[ScreenConstants::bottomScreen].data = (u8*) calloc(65536, sizeof(u8));
    NF_Flip8bitsBackBuffer(ScreenConstants::topScreen,1);
    free(NF_8BITS_BACKBUFFER[ScreenConstants::topScreen].data);
    NF_8BITS_BACKBUFFER[ScreenConstants::topScreen].data = (u8*) calloc(65536, sizeof(u8));
}

void ClearScreen(){
    for(int screen : {0,1}){
        for(int layer : {0,1,2,3}){
            if(NF_TILEDBG_LAYERS [screen][layer].created){
                NF_DeleteTiledBg (screen, layer);
            }
        }
    }
    UpdateScreen();
    CM_DestroyAllSprites();
    bgSetPriority(0,3);
    bgSetPriority(3,0);
    bgSetPriority(4,3);
    bgSetPriority(7,0);
}

void DrawLine(int screen, Pos end1,Pos end2){
    int dx = end2.x - end1.x;
    int dy = end2.y - end1.y;
    int steps = std::max(std::abs(dx),std::abs(dy));
    float xIncrement = dx / (float)steps;
    float yIncrement = dy / (float)steps;
    float x = end1.x;
    float y = end1.y;
    for (int i = 0; i < steps; i++) {
        NF_8BITS_BACKBUFFER[screen].data[(int)x + (int)y * 256] = 2;
        x += xIncrement;
        y += yIncrement;
    }
}


void DrawBox(int screen, Pos corner, Pos opposingCorner){
    // Determine the bounding box
    u16 minX = (corner.x < opposingCorner.x) ? corner.x : opposingCorner.x;
    u16 maxX = (corner.x > opposingCorner.x) ? corner.x : opposingCorner.x;
    u16 minY = (corner.y < opposingCorner.y) ? corner.y : opposingCorner.y;
    u16 maxY = (corner.y > opposingCorner.y) ? corner.y : opposingCorner.y;

    // Draw the top and bottom horizontal lines
    for (u16 i = minX; i <= maxX; i++) {
        if((abs(corner.x-i)%5)%2==0){ //Dotted line
            NF_8BITS_BACKBUFFER[screen].data[i + (minY * 256)] = 2; // Top
            NF_8BITS_BACKBUFFER[screen].data[i + (maxY * 256)] = 2; // Bottom
        }
    }

    // Draw the left and right vertical lines
    for (u16 i = minY; i <= maxY; i++) {
        if((abs(corner.x-i)%5)%2==0){ //Dotted line
            NF_8BITS_BACKBUFFER[screen].data[minX + (i * 256)] = 2; // Left
            NF_8BITS_BACKBUFFER[screen].data[maxX + (i * 256)] = 2; // Right
        }
    }
}