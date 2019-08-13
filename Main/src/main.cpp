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
#include "Utils/Utils.hpp"
#include "Utils/Logger.hpp"




int runNestest();
int run_instr_test_v5();
int livePlay(char*, int spritesheet);

static void hexDump(Core &cpu, unsigned short start, unsigned short end);
static void hexDumpCPU(Mapper& m, unsigned short start, unsigned short end);
static void hexDumpPPU(Mapper& m, unsigned short start, unsigned short end);

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

struct controllerButton {
    buttons button;
    int key;
    std::string buttonName;
};

controllerButton keyMappings[0x8] = {
    {buttons::A, SDL_SCANCODE_X, "A"},
    {buttons::B, SDL_SCANCODE_Z, "B"},
    {buttons::SELECT, SDL_SCANCODE_RSHIFT, "SELECT"},
    {buttons::START, SDL_SCANCODE_RETURN, "START"},
    {buttons::UP, SDL_SCANCODE_UP, "UP"},
    {buttons::DOWN, SDL_SCANCODE_DOWN, "DOWN"},
    {buttons::LEFT, SDL_SCANCODE_LEFT, "LEFT"},
    {buttons::RIGHT, SDL_SCANCODE_RIGHT, "RIGHT"}
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
    
    Log::g_filter = Log::Level::Main;
    
//    Mapper000 map;
//    std::ifstream file ("../../Roms/nestest.nes", std::ios::in|std::ios::binary|std::ios::ate);
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
////    hexDumpPPU(map, 0x2000, 0x2400);
////    ppu.renderNametable(0x2000, 0);
//    ppu.renderSpritesheet(0);
//    SDL_RenderPresent(g_renderer);
//
//    holdWindowOpen();
//    window_close();

//    unsigned int g_filter = Log::Level::Controller;
    
//    auto ret = livePlay("../../Roms/nestest.nes", 0);
    auto ret = livePlay("../../Roms/Tetris.nes", 7);

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

int livePlay(char* gameName, int spritesheet) {
    Mapper001 map;
    std::ifstream file (gameName, std::ios::in|std::ios::binary|std::ios::ate);
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
        //poll for input
        startTime = SDL_GetTicks();
        while (SDL_PollEvent(&e)){
            if (e.type == SDL_QUIT){
                quit = true;
            }
            if (e.type == SDL_MOUSEBUTTONDOWN){
                quit = false;
            }
        }
        
        //populate controller input
        const Uint8 *state = SDL_GetKeyboardState(NULL);
        for(controllerButton pair : keyMappings){
            if(state[pair.key]){
                map.orButtonValue(pair.button);
                logf(Log::Level::Controller, "Key Pressed: %s\n", pair.buttonName.c_str());
            }
        }
        
        //every frame, outside of polling
        std::cout << "--------Frame " << frame << "--------" << std::endl;

        // Run Frame
        
        map.clearVBlank();
        status = cpu.stepTo(frame * STEPS_PER_FRAME + VBLANK_START);
        if(status != 0){
            printf("[Main][Error]BRK\n");
        }
        cpu.setNMI();
        map.setVBlank();
        cpu.stepTo(frame * STEPS_PER_FRAME + STEPS_PER_FRAME);

        // Draw
        
        SDL_RenderClear(g_renderer);
        ppu.renderNametable(map.getPPUPointerAt(0x2000), spritesheet); // tetris 7
//        hexDumpPPU(map, 0x2000, 0x2400);
//        ppu.renderNametable(0x2000, 7);
//        ppu.renderNametable(map.VRAM, 0);
        ppu.renderSprites(spritesheet); // tetris 7
//        ppu.renderSpritesheet(0);
        SDL_RenderPresent(g_renderer);

        frame++;

        Uint32 currentTime = SDL_GetTicks();
        Uint32 frameTicks = currentTime - startTime;
        if(frameTicks < SCREEN_TICKS_PER_FRAME){
            Uint32 delay = SCREEN_TICKS_PER_FRAME - frameTicks;
            logf(Log::Level::Main, "Delaying %u\n", delay);
            SDL_Delay(delay);
        }

        //Post Draw
        
        map.setButtonValue(0);
    }

    window_close();
    return 0;
}
//---------------------------------------------------------------------
int runPPUTestOne() {
    Mapper001 map;
    std::ifstream file ("Tetris.nes", std::ios::in|std::ios::binary|std::ios::ate);
    if(!file.is_open()){
        std::cout << "Unable to open file";
        return 1;
    }
    map.readINES(file);
    file.close();

    std::ifstream ppuDumpFile ("ppuGreen.b", std::ios::in|std::ios::binary|std::ios::ate);
    if(!ppuDumpFile.is_open()){
        std::cout << "unable to open ppu dump";
        return 1;
    }
    char ppuDump[0x4000];
    ppuDumpFile.seekg(0, std::ios::beg);
    ppuDumpFile.read(ppuDump, 0x4000);
    ppuDumpFile.close();

    memcpy(map.getPPUPointerAt(0x2000), ppuDump + 0x2000, 0x2000);

    std::ifstream oamDumpFile ("tetrisOAM.b", std::ios::in|std::ios::binary|std::ios::ate);
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
            printf("%02x ", Utils::printable(cpu.m.getByte(i + j)));
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
            printf("%02x ", Utils::printable(m.getByte(i + j)));
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
            printf("%02x ", Utils::printable(m.getPPU(i + j)));
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

    std::ifstream file ("official_only.nes", std::ios::in|std::ios::binary|std::ios::ate);
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

    std::ifstream file ("nestest.nes", std::ios::in|std::ios::binary|std::ios::ate);
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
