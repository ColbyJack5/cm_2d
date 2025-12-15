#include <dsf.h>    
#include <nds.h>
#include <nf_lib.h>
#include "font2_0_16.h"
#include "font2_fnt_bin.h"
#include "blocky_0.h"
#include <constants.h>
#include <cstring>
#include "misc.h"


struct Font
{
    int fontIMGwidth;
    int fontIMGheight;
    int textureFormat;
    /* data */
};



dsf_handle handle;


void CM_LoadFont(const void* fontData, int fontDataSize){
    dsf_error rc = DSF_LoadFontMemory(&handle, fontData, fontDataSize);
    if (rc != DSF_NO_ERROR)
    {
        NF_Error(200,"Failed to load font", 200);
    }
}

void CM_LoadFont(const char* path){
    dsf_error rc = DSF_LoadFontFilesystem(&handle, path);
    if (rc != DSF_NO_ERROR)
    {
        NF_Error(200,"Failed to load font", 200);
    }
}

void CM_DrawText(int Screen,char* text, Pos position){

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
        if (lineCount >= 50) {
            wrappedtext[lastSpace] = '\n';
            lineCount = -1;
        }
        lineCount++;
    }
    
    //Create a Texture from the font data
    size_t out_width, out_height;

    void *out_texture;
    dsf_error error = DSF_StringRenderToTexture(handle,
                        wrappedtext, GL_RGB16,
                        blocky_0Bitmap, 128, 128,
                        &out_texture, &out_width, &out_height);
    if (error != DSF_NO_ERROR)
    {
        NF_Error(201,"Failed to generate texture", 201);
    }

    //Draw to the BackBuffer
    for (u16 i = 0; i < out_height; i++){ //y
        for (u16 j = 0; j < out_width; j+=2){  //x
            // Index into the texture data
            int byte_index = (i * (out_width / 2)) + (j / 2);
            u8 byte = static_cast<u8*>(out_texture)[byte_index];

            // Decode the two texels
            u8 texel1 = byte & 0x0F;        // Lower 4 bits
            u8 texel2 = (byte >> 4) & 0x0F; // Upper 4 bits
            int screen_x1 = position.x + j;
            int screen_x2 = position.x + j + 1;
            int screen_y  = position.y + i;

            // Write colors to the back buffer
            if(texel1>0){
                NF_8BITS_BACKBUFFER[Screen].data[screen_x1 + (screen_y * 256)] = 1;
            }
            if(texel2>0){
                NF_8BITS_BACKBUFFER[Screen].data[screen_x2 + (screen_y * 256)] = 1;
            }
        }
    }
    free(out_texture);
}