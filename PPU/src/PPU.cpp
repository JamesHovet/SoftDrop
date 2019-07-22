//
//  PPU.cpp
//  SoftDrop
//
//  Created by James Hovet on 7/20/19.
//  Copyright © 2019 James Hovet. All rights reserved.
//

#include "PPU.hpp"

const SDL_Color colors[0x40] = {
    SDL_Color{84,84,84,255},
    SDL_Color{0,30,116,255},
    SDL_Color{8,16,144,255},
    SDL_Color{48,0,136,255},
    SDL_Color{68,0,100,255},
    SDL_Color{92,0,48,255},
    SDL_Color{84,4,0,255},
    SDL_Color{60,24,0,255},
    SDL_Color{32,42,0,255},
    SDL_Color{8,58,0,255},
    SDL_Color{0,64,0,255},
    SDL_Color{0,60,0,255},
    SDL_Color{0,50,60,255},
    SDL_Color{0,0,0,255},
    SDL_Color{0,0,0,255},
    SDL_Color{0,0,0,255},
    SDL_Color{152,150,152,255},
    SDL_Color{8,76,196,255},
    SDL_Color{48,50,236,255},
    SDL_Color{92,30,228,255},
    SDL_Color{136,20,176,255},
    SDL_Color{160,20,100,255},
    SDL_Color{152,34,32,255},
    SDL_Color{120,60,0,255},
    SDL_Color{84,90,0,255},
    SDL_Color{40,114,0,255},
    SDL_Color{8,124,0,255},
    SDL_Color{0,118,40,255},
    SDL_Color{0,102,120,255},
    SDL_Color{0,0,0,255},
    SDL_Color{0,0,0,255},
    SDL_Color{0,0,0,255},
    SDL_Color{236,238,236,255},
    SDL_Color{76,154,236,255},
    SDL_Color{120,124,236,255},
    SDL_Color{176,98,236,255},
    SDL_Color{228,84,236,255},
    SDL_Color{236,88,180,255},
    SDL_Color{236,106,100,255},
    SDL_Color{212,136,32,255},
    SDL_Color{160,170,0,255},
    SDL_Color{116,196,0,255},
    SDL_Color{76,208,32,255},
    SDL_Color{56,204,108,255},
    SDL_Color{56,180,204,255},
    SDL_Color{60,60,60,255},
    SDL_Color{0,0,0,255},
    SDL_Color{0,0,0,255},
    SDL_Color{236,238,236,255},
    SDL_Color{168,204,236,255},
    SDL_Color{188,188,236,255},
    SDL_Color{212,178,236,255},
    SDL_Color{236,174,236,255},
    SDL_Color{236,174,212,255},
    SDL_Color{236,180,176,255},
    SDL_Color{228,196,144,255},
    SDL_Color{204,210,120,255},
    SDL_Color{180,222,120,255},
    SDL_Color{168,226,144,255},
    SDL_Color{152,226,180,255},
    SDL_Color{160,214,228,255},
    SDL_Color{160,162,160,255},
    SDL_Color{0,0,0,255},
    SDL_Color{0,0,0,255}
};

PPU::PPU(Mapper& mapper, SDL_Renderer* renderer):map(mapper){
    ppuRenderer = renderer;
    numStaticSpritesheets = mapper.header[5];
    staticSpritesheets = new SDL_Surface*[numStaticSpritesheets];
    generateStaticSpritesheets();
}

PPU::~PPU(){
    SDL_DestroyRenderer(ppuRenderer);
    ppuRenderer = NULL;
    for(int i = numStaticSpritesheets - 1; i >=0 ; i--){
        SDL_FreeSurface(staticSpritesheets[i]);
    }
    delete[] staticSpritesheets;
}

void PPU::generateStaticSpritesheets(){
    const int NUM_TILES = 0x10 * 0x10 * numStaticSpritesheets;
    unsigned char tiles[NUM_TILES][0x40];
    unsigned char* chr = (unsigned char*)map.CHR;
    
    for(int t = 0; t < NUM_TILES; t++){
        for(int i = 0; i < 0x8; i++){
            for(int j = 0; j < 0x8; j++){
                tiles[t][i * 8 + j] = ((*chr >> (0x7 - j)) & '\x1') +
                (((*(chr + 8) >> (0x7 - j)) & '\x01') << 1);
            }
            chr += 0x1;
        }
        chr += 0x8;
    }
    
    for(int i = 0; i < numStaticSpritesheets; i++){
        staticSpritesheets[i] = SDL_CreateRGBSurfaceWithFormat(0, 128, 128, 8, SDL_PIXELFORMAT_INDEX8);
    }
    
    SDL_Surface* spriteSurface = SDL_CreateRGBSurfaceWithFormatFrom(tiles, 8, 8, 8, 8, SDL_PIXELFORMAT_INDEX8);
    SDL_Rect dst; dst.w = 8; dst.h = 8;
    for(int i = 0; i < NUM_TILES; i++){
        spriteSurface->pixels = tiles[i];
        dst.x = (8 * i) % 128;
        dst.y = ((i % 256) / 16) * 8;
        SDL_BlitSurface(spriteSurface, NULL, staticSpritesheets[i / 256], &dst);
    }
    SDL_FreeSurface(spriteSurface);

}

void PPU::renderSpritesheet(int sheet){
    SDL_Color debugTmpPaletteColors[4] = {
        SDL_Color{0,0,0,0},
        SDL_Color{255,0,0},
        SDL_Color{255,255,255},
        SDL_Color{0,0,255}
    };
    SDL_SetPaletteColors(staticSpritesheets[sheet]->format->palette, debugTmpPaletteColors, 0, 4);
    
    SDL_Surface* spritesheetSurfaceRGB24 = SDL_CreateRGBSurfaceWithFormat(0, 128, 128, 24, SDL_PIXELFORMAT_RGB24);
    
    SDL_BlitSurface(staticSpritesheets[sheet], NULL, spritesheetSurfaceRGB24, NULL);
    
    SDL_Texture* spritesheetTexture = SDL_CreateTextureFromSurface(ppuRenderer, spritesheetSurfaceRGB24);
    SDL_RenderCopy(ppuRenderer, spritesheetTexture, NULL, NULL);
    
    SDL_DestroyTexture(spritesheetTexture);
    SDL_FreeSurface(spritesheetSurfaceRGB24);
    
}

void PPU::renderNametable(char *begin, int sheetNumber){
    const int numTilesX = 256/8;
    const int numTilesY = 240/8;
    unsigned char* tile = (unsigned char*)begin;
    unsigned char* attributeTable = (unsigned char*)begin + 0x3c0;
    
    SDL_Surface* currentSpritesheet = staticSpritesheets[sheetNumber];
    
    SDL_Surface* output = SDL_CreateRGBSurfaceWithFormat(0, 256, 240, 32, SDL_PIXELFORMAT_RGBA32);
    SDL_Texture* outputTexture;
    
    SDL_Rect src{0,0,8,8};
    SDL_Rect dst{0,0,8,8};
    
    for(int i = 0; i < numTilesX * numTilesY; i ++){
        int cell = ((i / 128) * 8) + ((i % 32) / 4);
        int shift = i % 4 >= 2 ? 2 : 0;
        if(i % 4 >= 2){
            shift = 2;
        } else {
            shift = 0;
        }
        if(i % 128 >= 64){
            shift += 4;
        }
        int paletteNumber = (*(attributeTable + cell) >> shift) & '\x03';
        
        int tileNumber = *tile;
        
        src.x = 8 * (tileNumber % 16);
        src.y = 8 * (tileNumber / 16);
        
        dst.x = 8 * (i % numTilesX);
        dst.y = 8 * (i / numTilesX);
        
//        printf("%3d,%3d\tt:%02x\tp:%02x\tc:%3d\n", dst.x, dst.y, tileNumber,paletteNumber, cell);
        
        setTmpPaletteColors(paletteNumber);
        SDL_SetPaletteColors(currentSpritesheet->format->palette, &tmpColors[0], 0, 4);
        SDL_BlitSurface(currentSpritesheet, &src, output, &dst);
        
        tile += 1;
    }
    outputTexture = SDL_CreateTextureFromSurface(ppuRenderer, output);
    SDL_RenderCopy(ppuRenderer, outputTexture, NULL, NULL);
    
    SDL_FreeSurface(output);
    SDL_DestroyTexture(outputTexture);
    
}

void PPU::renderSprites(){
    SDL_Surface* output = SDL_CreateRGBSurfaceWithFormat(0, 256, 240, 32, SDL_PIXELFORMAT_RGBA32);
    SDL_FillRect(output, NULL, 0);
    SDL_Texture* outputTexture;
    
    SDL_Rect src{0,0,8,8};
    SDL_Rect dst{0,0,8,8};
    
    unsigned char* head = (unsigned char*)map.getOAM();
    for(int i = 0; i < 0x100; i += 4){
        if(*head >= 0xef){
            head += 4;
            continue;
        }
        unsigned char yPos = *(head + 0);
        unsigned char tileIndex = *(head + 1);
        unsigned char attributes = *(head + 2);
        unsigned char xPos = *(head + 3);
        
        src.x = 8 * (tileIndex % 16);
        src.y = 8 * (tileIndex / 16);
        
        dst.x = xPos;
        dst.y = yPos + 1;
        
        setTmpPaletteColors((attributes & '\x03') + 4);
        
        SDL_SetPaletteColors(staticSpritesheets[3]->format->palette, tmpColors, 0, 4);
        SDL_BlitSurface(staticSpritesheets[3], &src, output, &dst);
        head += 4;
    }
    outputTexture = SDL_CreateTextureFromSurface(ppuRenderer, output);
    SDL_SetTextureBlendMode(outputTexture, SDL_BLENDMODE_BLEND);
    SDL_RenderCopy(ppuRenderer, outputTexture, NULL, NULL);
    
    SDL_FreeSurface(output);
    SDL_DestroyTexture(outputTexture);
}

void PPU::setTmpPaletteColors(int paletteNumber){
    unsigned short firstAddr = 0x3f01 + (0x4 * paletteNumber);
    tmpColors[0] = colors[map.getPPU(0x3f00)]; // universal background
    for(int i = 0; i < 3; i++){
        unsigned char colorIndex = (unsigned char)map.getPPU(firstAddr + i);
        tmpColors[i + 1] = colors[colorIndex];
    }
}

void PPU::renderAllColors(){
    SDL_Surface* outputSurface = SDL_CreateRGBSurface(0, 128, 128, 32, 0, 0, 0, 0);
    SDL_Rect dst = {0,0,8, 32};
    for(int i = 0; i < 0x40; i++){
        dst.x = (i % 16) * 8;
        dst.y = (i / 16) * 32;
        SDL_Color c = colors[i];
        Uint32 c_u = SDL_MapRGBA(outputSurface->format, c.r, c.g, c.b, c.a);
        SDL_FillRect(outputSurface, &dst, c_u);
    }
    
    SDL_Texture* outputTexture = SDL_CreateTextureFromSurface(ppuRenderer, outputSurface);
    
    SDL_RenderCopy(ppuRenderer, outputTexture, NULL, NULL);
    
    SDL_DestroyTexture(outputTexture);
    SDL_FreeSurface(outputSurface);
}
