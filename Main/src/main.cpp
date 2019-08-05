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
int livePlay(char*);

static void hexDump(Core &cpu, unsigned short start, unsigned short end);
static void hexDumpCPU(Mapper& m, unsigned short start, unsigned short end);
static void hexDumpPPU(Mapper& m, unsigned short start, unsigned short end);
static int printable(char byte){return (((unsigned short)byte)&0xff);}

enum buttons {
    A = '\x01',
    B = '\x02',
    SELECT = '\x04',
    START = '\x08',
    UP = '\x10',
    DOWN = '\x20',
    LEFT = '\x40',
    RIGHT = '\x80'
};

bool window_init();
void window_close();
int runPPUTestOne();
void holdWindowOpen();

const int SCREEN_WIDTH = 128;
const int SCREEN_HEIGHT = 128;

SDL_Window* g_window = NULL;
SDL_Renderer* g_renderer = NULL;




int main(int argc, const char * argv[]) {
    
//    Mapper000 map;
//    ifstream file ("nestest.nes", std::ios::in|std::ios::binary|std::ios::ate);
//    if(!file.is_open()){
//        std::cout << "Unable to open file";
//        return 1;
//    }
//    map.readINES(file);
//    file.close();
//
//    window_init();
//
//    PPU ppu = PPU(map, g_renderer);
//
//    map.setByte(0x2006, '\x20');
//    map.setByte(0x2006, '\x00');
//    for(int i = 0; i < 0x400; i++){
//        map.setByte(0x2007, ((i % 32) + (i / 32)) % 32 + 0x41);
//    }
//    SDL_RenderClear(g_renderer);
//    hexDumpPPU(map, 0x2000, 0x2400);
//    ppu.renderNametable(0x2000, 0);
////    ppu.renderSpritesheet(0);
//    SDL_RenderPresent(g_renderer);
//
//    holdWindowOpen();
//    window_close();
    
//    auto ret = livePlay("nestest.nes");
    auto ret = livePlay("Tetris.nes");
    
//    auto ret = runPPUTestOne();
//    auto ret = runNestest();
//    auto ret = run_instr_test_v5();

    return ret;
    
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
        g_renderer = SDL_CreateRenderer(g_window, -1, SDL_RENDERER_ACCELERATED);
        
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

//---------------------------------------------------------------------

int livePlay(char* gameName) {
    Mapper001 map;
    ifstream file (gameName, std::ios::in|std::ios::binary|std::ios::ate);
    if(!file.is_open()){
        std::cout << "Unable to open file";
        return 1;
    }
    map.readINES(file);
    file.close();
    
    window_init();
    
    Core cpu = Core(map);
    int status = 0;
    PPU ppu = PPU(map, g_renderer);
    
    cpu.reset();
    
    hexDumpPPU(map, 0x2000, 0x2400);
    
    int STEPS_PER_FRAME = 29781;
    int VBLANK_START = 27393;

    int frame = 0;
    
    const int SCREEN_FPS = 60;
    const int SCREEN_TICKS_PER_FRAME = 1000 / SCREEN_FPS;
    
    Uint32 startTime = SDL_GetTicks();
    
    SDL_Event e;
    bool quit = false;
    while (!quit){
        startTime = SDL_GetTicks();
        while (SDL_PollEvent(&e)){
            if (e.type == SDL_QUIT){
                quit = true;
            }
            if (e.type == SDL_KEYDOWN){
                if(e.key.keysym.sym == SDLK_SPACE){
                    //                    std::cout << "--------Frame " << frame << "--------" << std::endl;
                    //
                    //                    map.clearVBlank();
                    //                    cpu.stepTo(frame * STEPS_PER_FRAME + VBLANK_START);
                    //                    cpu.setNMI();
                    //                    map.setVBlank();
                    //                    cpu.stepTo(frame * STEPS_PER_FRAME + STEPS_PER_FRAME);
                    //
                    //
                    //                    SDL_RenderClear(g_renderer);
                    //                    ppu.renderNametable(map.getPPUPointerAt(0x2000), 1);
                    ////                    ppu.renderNametable(map.VRAM, 0);
                    ////                    ppu.renderSprites(0);
                    ////                    ppu.renderSpritesheet(1);
                    //                    SDL_RenderPresent(g_renderer);
                    //
                    //                    frame++;
                    
                    //                    hexDumpPPU(map, 0x2000, 0x2400);
                } else if(e.key.keysym.sym == SDLK_RETURN){
                    map.orButtonValue(buttons::START);
                } else if(e.key.keysym.sym == SDLK_RSHIFT){
                    map.orButtonValue(buttons::SELECT);
                } else if(e.key.keysym.sym == SDLK_x){
                    map.orButtonValue(buttons::A);
                } else if(e.key.keysym.sym == SDLK_z){
                    map.orButtonValue(buttons::B);
                } else if(e.key.keysym.sym == SDLK_UP){
                    map.orButtonValue(buttons::UP);
                } else if(e.key.keysym.sym == SDLK_DOWN){
                    map.orButtonValue(buttons::DOWN);
                } else if(e.key.keysym.sym == SDLK_LEFT){
                    map.orButtonValue(buttons::LEFT);
                } else if(e.key.keysym.sym == SDLK_RIGHT){
                    map.orButtonValue(buttons::RIGHT);
                }
            }
            if (e.type == SDL_MOUSEBUTTONDOWN){
                quit = false;
            }
        }
        //every frame, outside of polling
        std::cout << "--------Frame " << frame << "--------" << std::endl;
        
        map.clearVBlank();
        status = cpu.stepTo(frame * STEPS_PER_FRAME + VBLANK_START);
        if(status != 0){
            printf("[Main][Error]BRK\n");
        }
        cpu.setNMI();
        map.setVBlank();
        cpu.stepTo(frame * STEPS_PER_FRAME + STEPS_PER_FRAME);
        
        
        SDL_RenderClear(g_renderer);
        ppu.renderNametable(map.getPPUPointerAt(0x2000), 7); // tetris 7
//        hexDumpPPU(map, 0x2000, 0x2400);
//        ppu.renderNametable(0x2000, 7);
//        ppu.renderNametable(map.VRAM, 0);
        ppu.renderSprites(7); // tetris 7
//        ppu.renderSpritesheet(0);
        SDL_RenderPresent(g_renderer);
        
        frame++;
        
        Uint32 currentTime = SDL_GetTicks();
        Uint32 frameTicks = currentTime - startTime;
        if(frameTicks < SCREEN_TICKS_PER_FRAME){
            Uint32 delay = SCREEN_TICKS_PER_FRAME - frameTicks;
            printf("[Main]\tDelaying %ul\n", delay);
            SDL_Delay(delay);
        }
        
    }
    
    window_close();
    return 0;
}
//---------------------------------------------------------------------
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
