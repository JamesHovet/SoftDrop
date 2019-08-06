//
//  Logger.cpp
//  SoftDrop
//
//  Created by James Hovet on 8/5/19.
//  Copyright © 2019 James Hovet. All rights reserved.
//

#include "Logger.hpp"

const char* levelStrings[] = {
    "Error",
    "Main",
    "Core",
    "Mapper",
    "PPU",
    "Controller",
    "SubMapper"
};

unsigned int Log::g_filter = Log::Level::SubMapper | Log::Level::Mapper;

void logf(unsigned int filters, const char* fmt, ...){
    
    if(filters & Log::g_filter){
        for(int i = 1; i < 16; i++){
            if((1 << i) & filters){
                std::cout << "[" << levelStrings[i] << "]";
            }
        }
        std::cout << "\t";
        va_list arg;
        va_start(arg, fmt);
        vprintf(fmt, arg);
        va_end(arg);
    }
}
