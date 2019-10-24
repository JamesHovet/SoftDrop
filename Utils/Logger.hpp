//
//  Logger.hpp
//  SoftDrop
//
//  Created by James Hovet on 8/5/19.
//  Copyright © 2019 James Hovet. All rights reserved.
//



#ifndef Logger_hpp
#define Logger_hpp

#include <stdio.h>
#include <stdarg.h>
#include <string>
#include <iostream>
#include <map>

#define MAX_IMGUI_LOG_LENGTH 256

namespace Log{
    
    extern unsigned int g_filter;
    
    enum Level : unsigned int
    {
        Error = 0x1,
        Main = 0x2,
        Core = 0x4,
        Mapper = 0x8,
        PPU = 0x10,
        Controller = 0x20,
        SubMapper = 0x40
    };
    
    struct logLevelString {
        const char* name;
        Log::Level level;
    };
    
    const logLevelString logLevelStrings[] {
        {"Error",Log::Level::Error},
        {"Main",Log::Level::Main},
        {"Core",Log::Level::Core},
        {"Mapper",Log::Level::Mapper},
        {"PPU",Log::Level::PPU},
        {"Controller",Log::Level::Controller},
        {"SubMapper",Log::Level::SubMapper},
    };
    
    const std::map<std::string, logLevelString> logLevelStringMap {
        {"Error",{"Error",Log::Level::Error}},
        {"Main",{"Main",Log::Level::Main}},
        {"Core",{"Core",Log::Level::Core}},
        {"Mapper",{"Mapper",Log::Level::Mapper}},
        {"PPU",{"PPU",Log::Level::PPU}},
        {"Controller",{"Controller",Log::Level::Controller}},
        {"SubMapper",{"SubMapper",Log::Level::SubMapper}},
    };
    
    inline bool doesPassFilter(unsigned int filter){
        return filter & g_filter;
    }
}

void logf(unsigned int filters, const char* fmt, ...);

#endif /* Logger_hpp */
