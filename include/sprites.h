#pragma once
#include <string>
#include <vector>
#include "misc.h"

/* Creates a Sprite Template (256 Max) that can be added to the VRAM of a screen in order to create sprites. 
    Set isAnimated to false in order to animate each sprite instance seperatly otherwise they will all be set to the same frame*/
Identifier CM_CreateSpriteTemplate(const std::string& name, int width, int height, bool isAnimated);


/* Adds an already created Sprite Template to the VRAM (128 Max) of a screen (0-1) in order to create a sprite. Make sure to do this prior to 
    creating a sprite on each screen */
void CM_LoadSpriteTemplateToVRAM(int screen, const std::string& templateName);


/* Removes a Sprite Template from the VRAM of a screen (0-1) in order to make room for others */
void CM_UnloadSpriteTemplateFromVRAM(int screen, const std::string& templateName);


/* Deletes a Sprite Template in order to make space for others. Unlikely you will have to use this except for very large games */
void CM_DeleteSpriteTemplate(const std::string& templateName);


/* Add an Animation to a Sprite Template. This can be called before or after adding the template to VRAM. The animation is a list of 
the frames in the order they are meant to appear. The frame delay is how many frames (60fps) each frame will appear. */
void CM_AddAnimationToTemplate(const std::string& templateName, const std::string& animationName, const std::vector<int>& frames, int frameDelay);

/* Creates a Sprite Palette (64 Max) that can be added to the VRAM of a screen in order to create sprites. */
Identifier CM_CreateSpritePalette(const std::string& paletteName);

/* Adds an already created Sprite Palette to the VRAM (15 Max) of a screen (0-1) in order to create a sprte. Make sure to do this prior to creating a sprite on each screen*/
void CM_LoadSpritePaletteToVRAM(int screen, const std::string& paletteName);

/* Removes a Sprite Palette from the VRAM of a screen (0-1) in order to make room for others */
void CM_UnloadSpritePaletteFromVRAM(int screen, const std::string& paletteName);

/* Deletes a Sprite Palette in order to make space for others. Unlikely you will have to use this except for very large games */
void CM_DeleteSpritePalette(const std::string& paletteName);
        
/* Creates an instance of a sprite on the specified screen (0-1) using a Sprite Template and position (x (0-255), y (0-192)). Keep ahold of this Identifier for later */
Identifier CM_CreateSprite(int screen, const std::string& templateName, Pos pos);


Identifier CM_CreateSprite(int screen, const std::string& templateName, const std::string& paletteName, Pos pos);

/* Returns whether or not an Identifier maps to an active Sprite Instance*/
bool CM_isSpriteValid(Identifier spriteIdentity);


/* Updates the animation of all Active Sprites. Call every frame. */
void CM_UpdateSprites();


/* Destroys all Active Sprites on both screens. Use to switch scenes */
void CM_DestroyAllSprites();


/* Destroys a specified sprite */
void CM_DestroySprite(Identifier spriteIdentity);


/* Moves Sprite to specified position (x (0-255), y (0-192)) */
void CM_MoveSprite(Identifier spriteIdentity, Pos position);


/* Sets the Animation of a Sprite. Animation must first be added to the Sprite Template the Sprite is created from */
void CM_SetSpriteAnimation(Identifier spriteIdentity, const std::string& animationName);

std::string CM_GetCurrentSpriteAnimation(Identifier spriteIdentity);

/* Gets whether or not the current animaiton is on its final frame */
bool CM_GetSpriteAnimationCompleted(Identifier spriteIdentity);

/* Adds an animation to the Queue that will play after the current animation has completed */
void CM_AddAnimationToSpriteQueue(Identifier spriteIdentity, const std::string& animationName);

/* Sets the render layer (0-3) of a sprite*/
void CM_SetSpriteLayer(Identifier spriteIdentity, int layer);


/* Sets the visibility of a sprite */
void CM_SetSpriteVisibility(Identifier spriteIdentity, bool isVisible);


/* Sets the horizontal flip of a sprite */
void CM_FlipSpriteHorizontally(Identifier spriteIdentity, bool isFlipped);


/* Retrives the horizontal flip of a sprite */
bool CM_GetSpriteHorizontalFlip(Identifier sprite);


/* Sets the vertical flip of a sprite */
void CM_FlipSpriteVertically(Identifier sprite, bool isFlipped);


/* Retrives the horizontal flip of a sprite */
bool CM_GetSpriteVerticalFlip(Identifier sprite);


/* Manually sets the frame of a Sprite. This will be over written if sprite is being animated */
void CM_SetSpriteAnimationFrame(Identifier sprite, int frame);


/* Creates A Rotation (-360° - 360°) and Scaler (0% - 200%) Group (32 Max) that a sprite can be bound to. */
Identifier CM_CreateRotScaleGroup(int screen, float rotation, float scaleX, float scaleY);


/* Binds a sprite to a RotScaleGroup. This will cause it to be rotated and scaled with the rest of the group */
void CM_BindSpriteToRotScaleGroup(Identifier sprite, Identifier group, bool doubleSize);


/* Removes a sprite from a RotScaleGroup causing its rotation and scale to return to normal */
void CM_RemoveSpriteFromRotScaleGroup(Identifier sprite, Identifier group);


/* Rotates all the sprites in a RotScaleGroup to a certian angle (-360° - 360°) */
void CM_RotateGroup(Identifier group, float rotation);


/* Scales all the sprites in a RotScaleGroup to a certain size (x (0% - 200%), y (0% - 200*))*/
void CM_ScaleGroup(Identifier group, float scaleX, float scaleY);


/* Rotates and scales all sprites in the group*/
void CM_RotateAndScaleGroup(Identifier group, float rotation, float scaleX, float scaleY);


/* Removes all the sprites in a RotScaleGroup causing their rotation and scale to return to normal */
void CM_RemoveAllSpritesFromRotScaleGroup(Identifier group);