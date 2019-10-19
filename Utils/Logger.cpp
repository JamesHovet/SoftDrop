//
//  Logger.cpp
//  SoftDrop
//
//  Created by James Hovet on 8/5/19.
//  Copyright © 2019 James Hovet. All rights reserved.
//

#include "Logger.hpp"

unsigned int Log::g_filter = 0;

/**
 send a log message to the log filterer. If the message passes the filter, it is outputted to
 cout.

 @param filters a bit mask of filters. eg) Log::Level::Mapper. Filters can be combined using
 bitwise or eg) Log::Level::Mapper | Log::Level::SubMapper
 @param fmt string format for the interal printf
 @param ... arguments for the internal printf
 */
void logf(unsigned int filters, const char* fmt, ...){
    if(filters & Log::g_filter){
        for(int i = 1; i < 16; i++){
            if((1 << i) & filters){
                std::cout << "[" << Log::logLevelStrings[i].name << "]";
            }
        }
        std::cout << "\t";
        va_list arg;
        va_start(arg, fmt);
        vprintf(fmt, arg);
        va_end(arg);
    }
}
