//
//  main.h
//  SoftDrop
//
//  Created by James Hovet on 7/14/19.
//  Copyright © 2019 James Hovet. All rights reserved.
//

#ifndef main_h
#define main_h

#include <iostream>
#include <fstream>

#include "libs/imgui/imgui.h"
#include "libs/imgui/imgui_impl_sdl.h"
//#include "libs/imgui/imgui_impl_opengl3.h"

#include <SDL2/SDL.h>
#include "libs/imgui/imgui_sdl.h"
#include <glew.h>

#include "boost/program_options.hpp"

#include "Core/src/Core.hpp"
#include "Core/src/Mapper.hpp"
#include "Core/src/Mappers/Mapper000.hpp"
#include "Core/src/Mappers/Mapper001.hpp"
#include "PPU/src/PPU.hpp"
#include "Utils/Utils.hpp"
#include "Utils/Logger.hpp"

#include "DebugOptions.hpp"

//buttons and button mapping
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

#endif /* main_h */
