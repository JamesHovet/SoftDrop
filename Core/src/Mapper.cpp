//
//  Mapper.cpp
//  SoftDrop
//
//  Created by James Hovet on 7/18/19.
//  Copyright © 2019 James Hovet. All rights reserved.
//

#include "Mapper.hpp"

void Mapper::setByte(unsigned short address, char byte){
    if(address >= 0x4000 && address < 0x4020){
        APUIO[address - 0x4000] = byte;
    } else if(address >= 0x2000 && address < 0x4000){
        PPU[address % 0x8] = byte;
    }
}

char Mapper::getByte(unsigned short address){
    if(address >= 0x4000 && address < 0x4020){
        return APUIO[address - 0x4000];
    } else if(address >= 0x2000 && address < 0x4000){
        char res = PPU[address % 0x8];
        if(address == 0x2002){
            PPU[0x2] = PPU[0x2] & '\x7f'; // clear bit 7 (VBlank) on read.
        }
        return res;
    }
    return 0;
}

char* Mapper::getPointerAt(unsigned short address){
    if(address >= 0x4000 && address < 0x4020){
        return &APUIO[address - 0x4000];
    } else if(address >= 0x2000 && address < 0x4000){
        return &PPU[address % 0x8];
    }
    return nullptr;
}
