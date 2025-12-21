#include <dsf.h>    
#include <nds.h>
#include <nf_lib.h>
#include <map>
#include <constants.h>
#include <cstring>
#include <string>
#include "misc.h"
#include "objectpool.h"
#include <iostream>


struct Font
{
    Identifier identity;
    std:: string name;
    dsf_handle handle;
    int GFXwidth;
    int GFXheight;
    int textureFormat;
    const void* bitmap;
    Font(Identifier identity, const std::string& fontName, int GFXwidth, int GFXheight, int textureFormat, const void* bitmap) : identity (identity), name (TrimPath(fontName)), GFXwidth (GFXwidth), GFXheight (GFXheight), textureFormat (textureFormat), bitmap(bitmap){
        dsf_error rc = DSF_LoadFontFilesystem(&this->handle, fontName.c_str());
        if (rc != DSF_NO_ERROR)
        {
            std::cout << "Failed to load font: " << name << std::endl;
        }
    }
    ~Font(){

    }
};

#define MaxFonts 3


ObjectPool<Font, MaxFonts> fonts;
std::map<std::string, Identifier> fontRegistry;


void CM_LoadFont(const char* path, int GFXwidth, int GFXheight, int textureFormat, const void* bitmap){
    Font* font = fonts.create(path, GFXwidth, GFXheight, textureFormat, bitmap);
    fontRegistry[TrimPath(path)] = font->identity;
}


void CM_DrawText(int Screen, char* text, const std::string& fontName, Pos position){

    int lineCount = 0;
    int lastSpace = 0;
    char wrappedtext[std::strlen(text)+1];
    std::strcpy(wrappedtext, text);
    for ( int i = 0; wrappedtext[i] != '\0'; i++) {  // Wrap text
        if (wrappedtext[i] == ' ') {
            lastSpace = i;
        }
        if(wrappedtext[i]=='\n'){
            lineCount=-1;
        }
        if (lineCount >= 40) {
            wrappedtext[lastSpace] = '\n';
            lineCount = -1;
        }
        lineCount++;
    }
    
    //Create a Texture from the font data
    size_t out_width, out_height;

    void *out_texture;
    if (fontRegistry.find(fontName) == fontRegistry.end()) {
        std::cout << "Font not found in registry: " << fontName << ". Registered Fonts: " << std::endl;
        for(auto fond :fontRegistry){
            std::cout << fond.first << ", " << std::endl;
        }
        return;
    }
    Font* font = fonts.get(fontRegistry[fontName]); 
    dsf_error error = DSF_StringRenderToTexture(font->handle,
                        wrappedtext, font->textureFormat,
                        font->bitmap, font->GFXwidth, font->GFXheight,
                        &out_texture, &out_width, &out_height);
    if (error != DSF_NO_ERROR)
    {
        NF_Error(201,"Failed to generate texture", 201);
    }


    //Draw to the BackBuffer
    u8* texData = static_cast<u8*>(out_texture);

    for (u16 i = 0; i < out_height; i++) {
        int screenYOffset = (position.y + i) * 256;
        int texYOffset = i * (out_width / 2);

        for (u16 j = 0; j < out_width; j += 2) {
            u8 byte = texData[texYOffset + (j / 2)];    // Index into the texture data
            u8 texel1 =  byte & 0x0F;                   // Lower 4 bits
            u8 texel2 =  (byte >> 4) & 0x0F;            // Upper 4 bits

            int screen_x1 = position.x + j;
            if (texel1 > 0) {
                NF_8BITS_BACKBUFFER[Screen].data[screen_x1 + screenYOffset] = 1; 
            }
            if (texel2 > 0) {
                NF_8BITS_BACKBUFFER[Screen].data[screen_x1 + 1 + screenYOffset] = 1;
            }
        }
    }
    free(out_texture);
}