//
//  main.cpp
//  SoftDrop
//
//  Created by James Hovet on 7/14/19.
//  Copyright © 2019 James Hovet. All rights reserved.
//

#define private public // EVIL HACK

#include "main.h"
#include <iostream>
#include <fstream>

#include <SDL2/SDL.h>

#include "Core/src/Core.hpp"
#include "Core/src/Mapper.hpp"
#include "Core/src/Mappers/Mapper000.hpp"
#include "Core/src/Mappers/Mapper001.hpp"

#include "PPU/src/PPU.hpp"

using namespace std;

int runNestest();
int run_instr_test_v5();
static void hexDump(Core &cpu, unsigned short start, unsigned short end);
static int printable(char byte){return (((unsigned short)byte)&0xff);}


bool init();
void close();
int runPPUTestOne();

const int SCREEN_WIDTH = 128;
const int SCREEN_HEIGHT = 128;

SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;


int main(int argc, const char * argv[]) {
    
    
    
    
//    auto ret = runPPUTestOne();
//    auto ret = runNestest();
//    auto ret = run_instr_test_v5();

//    return ret;
    
    return 0;
}

bool init()
{
    //Initialization flag
    bool success = true;
    
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, 0);
    
    //Initialize SDL
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
        printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
        success = false;
    }
    else
    {
        //Create window
        gWindow = SDL_CreateWindow("SoftDrop", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 256, 240, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
        gRenderer = SDL_CreateRenderer(gWindow, -1, 0);
        
        if( gWindow == NULL || gRenderer == NULL)
        {
            printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
            success = false;
        }
    }
    
    return success;
}


void close()
{
    SDL_DestroyRenderer(gRenderer);
    gRenderer = NULL;
    
    //Destroy window
    SDL_DestroyWindow( gWindow );
    gWindow = NULL;
    
    //Quit SDL subsystems
    SDL_Quit();
}














int runPPUTestOne() {
    Mapper001 map;
    ifstream file ("Tetris.nes", std::ios::in|std::ios::binary|std::ios::ate);
    if(!file.is_open()){
        std::cout << "Unable to open file";
        return 1;
    }
    map.readINES(file);
    file.close();
    
    ifstream ppuDumpFile ("ppuGreen.b", std::ios::in|std::ios::binary|std::ios::ate);
    if(!ppuDumpFile.is_open()){
        std::cout << "unable to open ppu dump";
        return 1;
    }
    char ppuDump[0x4000];
    ppuDumpFile.seekg(0, std::ios::beg);
    ppuDumpFile.read(ppuDump, 0x4000);
    ppuDumpFile.close();
    
    memcpy(map.getPPUPointerAt(0x2000), ppuDump + 0x2000, 0x2000);
    
    ifstream oamDumpFile ("tetrisOAM.b", std::ios::in|std::ios::binary|std::ios::ate);
    if(!oamDumpFile.is_open()){
        std::cout << "unable to open OAM dump";
        return 1;
    }
    oamDumpFile.seekg(0, std::ios::beg);
    oamDumpFile.read(map.OAM, 0x100);
    oamDumpFile.close();
    
    init();
    
    
    PPU ppu = PPU(map, gRenderer);
    //    ppu.renderSpritesheet(3);
    //    ppu.renderSpritesheet(map.getPPUPointerAt(0) + 0x1000 * 0);
    //    ppu.renderNametable(ppuDump + 0x2000, 3);
    //    ppu.renderAllColors();
    //    ppu.renderSprites();
    //    SDL_RenderPresent(gRenderer);
    
    
    SDL_Event e;
    bool quit = false;
    while (!quit){
        //        ppu.renderNametable(ppuDump + 0x2000, 3);
        //        ppu.renderSprites();
        SDL_RenderPresent(gRenderer);
        while (SDL_PollEvent(&e)){
            if (e.type == SDL_QUIT){
                quit = true;
            }
            if (e.type == SDL_KEYDOWN){
                quit = false;
            }
            if (e.type == SDL_MOUSEBUTTONDOWN){
                quit = false;
            }
        }
    }
    
    close();
    return 0;
}

static void hexDump(Core &cpu, unsigned short start, unsigned short end) {
    for(int i = start; i < end; i+=0x10){
        printf("$%04x: ", i);
        for(int j = 0; j < 0x10; j++){
            printf("%02x ", printable(cpu.m.getByte(i + j)));
        }
        for(int j = 0; j < 0x10; j++){
            char c = cpu.m.getByte(i + j);
            if(c > 31 && c < 127){
                std::cout << c;
            } else {
                std::cout << '.';
            }
        }
        printf("\n");
    }
}

int run_instr_test_v5() {
    //PC = ea71
    
    Mapper001 map;
    
    ifstream file ("official_only.nes", ios::in|ios::binary|ios::ate);
    if(!file.is_open()){
        std::cout << "Unable to open file";
        return 1;
    }
    map.readINES(file);
    file.close();
    
    
    Core cpu = Core(map);
    cpu.setPC(0xea71);


//    hexDump(cpu, 0xc000, 0xd000);

    long instructionCount = 0;
    while(cpu.step() == 0){
        if(instructionCount % 0x10000 == 0){
            std::cout << "------" << instructionCount << "------" << std::endl;
            hexDump(cpu, 0x6000, 0x6100);
            instructionCount += 0;
        }
        instructionCount++;
    }


    hexDump(cpu, 0x6000, 0x7000);

    return 0;
}

int runNestest(){
    Mapper000 map;
    
    ifstream file ("nestest.nes", ios::in|ios::binary|ios::ate);
    if(!file.is_open()){
        std::cout << "Unable to open file";
        return 1;
    }
    map.readINES(file);
    
    Core cpu = Core(map);
    cpu.setPC(0xc000); // automatic;
//    cpu.setPC(0xc004);
    
    return 0;
}
