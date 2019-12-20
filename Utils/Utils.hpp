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
    
    /**
     Get the value of bits A through B shifted down. For example, if you want
     the value of bits 2 through 4 inclusive of the byte 0b00000110,
     f(2,3) would return 0b00000001 or 1
     
     @param v the original byte
     @param lower the lowest bit of the range
     @param len the number of bits to get
     @return the shifted value of the bits in the range
     */
    static unsigned char getShiftedValueOfBitRange(unsigned char v, int lower, int len){
        unsigned char tmp = v << (8 - (lower + len));
        return tmp >> (8 - len);
    }
    
    static bool getBitInByte(unsigned char v, int bit){
        return v & (0x01 << bit);
    }
}

#endif /* Utils_hpp */
