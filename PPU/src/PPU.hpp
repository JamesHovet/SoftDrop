//
//  PPU.hpp
//  SoftDrop
//
//  Created by James Hovet on 7/20/19.
//  Copyright © 2019 James Hovet. All rights reserved.
//

#ifndef PPU_hpp
#define PPU_hpp

#include <iostream>
#include <SDL2/SDL.h>
#include "Core/src/Mapper.hpp"
#include "Utils/Logger.hpp"

class PPU{
  
private:
    Mapper& map;
    SDL_Renderer* ppuRenderer;
    
    int numStaticSpritesheets;
    SDL_Surface** staticSpritesheets;
    void generateStaticSpritesheets();
    
    void setTmpPaletteColors(int paletteNumber);
    SDL_Color tmpColors[4];
    
public:
    PPU(Mapper& mapper, SDL_Renderer* renderer);
    ~PPU();
    
    void renderSpritesheet(char* begin);
    void renderSpritesheet(int sheet);
    void renderNametable(char* begin, int sheetNumber);
    void renderNametable(unsigned short ppuAddress, int sheetNumber);
    void renderSprites(int sheetNumber);
    
    
    void renderAllColors();
    
    SDL_Color debugPaletteColors[4] = {
        SDL_Color{0,0,0,255},
        SDL_Color{255,0,0,255},
        SDL_Color{255,255,255,255},
        SDL_Color{0,0,255,255}
    };
    
    
};

#endif /* PPU_hpp */
