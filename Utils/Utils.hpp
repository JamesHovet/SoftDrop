//
//  Utils.hpp
//  SoftDrop
//
//  Created by James Hovet on 8/5/19.
//  Copyright © 2019 James Hovet. All rights reserved.
//

#ifndef Utils_hpp
#define Utils_hpp

#include <stdio.h>

namespace Utils {
    inline static int printable(char byte){
        return (((unsigned short)byte)&0xff);
    }
    
    /**
     converts a C string of hex characters of a given length to an unsigned
     int
     
     @param start the hex C string
     @param length the length of the passed C string
     @return the converted value of the passed hex string
     */
    static unsigned short convertHexStringToUnsignedShort(char* start, int length){
        unsigned int running = 0;
        int shift = 0;
        for(int i = length - 1; i >= 0; i--){
            if(start[i] <= '9' && start[i] >= '0'){
                running += (start[i] - '0') << (shift * 4);
                shift++;
            } else if (start[i] <= 'F' && start[i] >= 'A'){
                running += (start[i] - 'A' + 10) << (shift * 4);
                shift++;
            }
        }
        
        return running;
    }
}

#endif /* Utils_hpp */
