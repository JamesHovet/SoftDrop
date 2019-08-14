//
//  main.h
//  SoftDrop
//
//  Created by James Hovet on 7/14/19.
//  Copyright © 2019 James Hovet. All rights reserved.
//

#ifndef main_h
#define main_h

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
