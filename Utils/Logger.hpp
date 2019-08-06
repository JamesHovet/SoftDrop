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
}

void logf(unsigned int filters, const char* fmt, ...);

#endif /* Logger_hpp */
