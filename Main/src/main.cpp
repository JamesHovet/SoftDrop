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
static void hexDumpCPU(Mapper& m, unsigned short start, unsigned short end);
static void hexDumpPPU(Mapper& m, unsigned short start, unsigned short end);
static int printable(char byte){return (((unsigned short)byte)&0xff);}


bool window_init();
void window_close();
int runPPUTestOne();
void holdWindowOpen();

const int SCREEN_WIDTH = 128;
const int SCREEN_HEIGHT = 128;

SDL_Window* g_window = NULL;
SDL_Renderer* g_renderer = NULL;


int main(int argc, const char * argv[]) {
    
    Mapper000 map;
    ifstream file ("nestest.nes", std::ios::in|std::ios::binary|std::ios::ate);
    if(!file.is_open()){
        std::cout << "Unable to open file";
        return 1;
    }
    map.readINES(file);
    file.close();

    window_init();

    Core cpu = Core(map);
    PPU ppu = PPU(map, g_renderer);

    cpu.reset();

    hexDumpPPU(map, 0x2000, 0x2400);

    int STEPS_PER_FRAME = 30000;
    int STEPS_PER_NMI = 0;
    int frame = 1;

    SDL_Event e;
    bool quit = false;
    while (!quit){
        while (SDL_PollEvent(&e)){
            if (e.type == SDL_QUIT){
                quit = true;
            }
            if (e.type == SDL_KEYDOWN){
                if(e.key.keysym.sym == SDLK_SPACE){
                    std::cout << "--------Frame " << frame << "--------" << std::endl;
                    cpu.stepTo(frame * STEPS_PER_FRAME);
                    if(frame > 1){
                        cpu.setNMI();
                        map.setVBlank();
                    }

                    SDL_RenderClear(g_renderer);
//                    ppu.renderNametable(map.getPPUPointerAt(0x2000), 0);
                    ppu.renderNametable(map.VRAM, 0);
//                    ppu.renderSprites(0);
//                    ppu.renderSpritesheet(0);
                    SDL_RenderPresent(g_renderer);

                    frame++;

                    hexDumpPPU(map, 0x2000, 0x2400);
                } else if(e.key.keysym.sym == SDLK_RETURN){

                }
            }
            if (e.type == SDL_MOUSEBUTTONDOWN){
                quit = false;
            }
        }
    }

    window_close();
    
    
//    auto ret = runPPUTestOne();
//    auto ret = runNestest();
//    auto ret = run_instr_test_v5();

//    return ret;
    
    return 0;
}

bool window_init()
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
        g_window = SDL_CreateWindow("SoftDrop", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 256, 240, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
        g_renderer = SDL_CreateRenderer(g_window, -1, 0);
        
        if( g_window == NULL || g_renderer == NULL)
        {
            printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
            success = false;
        }
    }
    SDL_RenderClear(g_renderer);
    return success;
}


void window_close()
{
    SDL_DestroyRenderer(g_renderer);
    g_renderer = NULL;
    
    //Destroy window
    SDL_DestroyWindow( g_window );
    g_window = NULL;
    
    //Quit SDL subsystems
    SDL_Quit();
}













void holdWindowOpen() {
    SDL_Event e;
    bool quit = false;
    while (!quit){
        //        ppu.renderNametable(ppuDump + 0x2000, 3);
        //        ppu.renderSprites(3);
        //        SDL_RenderPresent(gRenderer);
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
    
    window_init();
    
    
    PPU ppu = PPU(map, g_renderer);
    //    ppu.renderSpritesheet(3);
    //    ppu.renderSpritesheet(map.getPPUPointerAt(0) + 0x1000 * 0);
        ppu.renderNametable(ppuDump + 0x2000, 3);
//        ppu.renderAllColors();
        ppu.renderSprites(3);
        SDL_RenderPresent(g_renderer);
    
    
    holdWindowOpen();
    
    window_close();
    return 0;
}

static void hexDump(Core &cpu, unsigned short start, unsigned short end) {
    for(int i = start; i < end; i+=0x10){
        printf("[Main]\t");
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

static void hexDumpCPU(Mapper& m, unsigned short start, unsigned short end) {
    for(int i = start; i < end; i+=0x10){
        printf("[Main]\t");
        printf("$%04x: ", i);
        for(int j = 0; j < 0x10; j++){
            printf("%02x ", printable(m.getByte(i + j)));
        }
        for(int j = 0; j < 0x10; j++){
            char c = m.getByte(i + j);
            if(c > 31 && c < 127){
                std::cout << c;
            } else {
                std::cout << '.';
            }
        }
        printf("\n");
    }
}

static void hexDumpPPU(Mapper& m, unsigned short start, unsigned short end) {
    for(int i = start; i < end; i+=0x10){
        printf("[Main]\t");
        printf("$%04x: ", i);
        for(int j = 0; j < 0x10; j++){
            printf("%02x ", printable(m.getPPU(i + j)));
        }
        for(int j = 0; j < 0x10; j++){
            char c = m.getPPU(i + j);
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
