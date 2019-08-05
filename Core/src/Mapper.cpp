//
//  Mapper.cpp
//  SoftDrop
//
//  Created by James Hovet on 7/18/19.
//  Copyright © 2019 James Hovet. All rights reserved.
//
#include "Mapper.hpp"

inline static int printable(char byte){
    return (((unsigned short)byte)&0xff);
}

void Mapper::setByte(unsigned short address, char byte){
    if(address < 0x2000){
        RAM[address] = byte;
    } else if(address >= 0x4000 && address < 0x4020){
        printf("[Mapper]\tWrote %x into APUIO address 0x%x\n", byte, address);
        APUIO[address - 0x4000] = byte;
    } else if(address >= 0x2000 && address < 0x4000){
        PPU[address % 0x8] = byte;
        printf("[Mapper]\tWrite %x to PPU Register %x\n", printable(byte), address);
        handlePPURegisterWrite(address, byte);
    }
}

char Mapper::getByte(unsigned short address){
    if(address == 0x4016){
        printf("[Mapper][Controller]\tbuttonValues:%x\n", printable(buttonValues));
        char val = buttonValues & '\x01';
        buttonValues = buttonValues >> 1;
        return val;
    } else if(address >= 0x2000 && address < 0x4000){
        address = 0x2000 + (address % 0x8);
        char res = handlePPURegisterRead(address);
        printf("[Mapper]\tRead %x from PPU Register %x\n", printable(res), address);
        return res;
    }
    
    return *getPointerAt(address);
}

char* Mapper::getPointerAt(unsigned short address){
    if(address < 0x2000){
        return &RAM[address];
    }else if(address >= 0x4000 && address < 0x4020){
        printf("[Mapper]\tRead* APUIO address 0x%x\n", address);
        return &APUIO[address - 0x4000];
    } else if(address >= 0x2000 && address < 0x4000){
        return &PPU[address % 0x8];
    }
    return nullptr;
}

void Mapper::setPPU(unsigned short address, char byte){
    if(address >= 0x2000){
        VRAM[address - 0x2000] = byte;
//        VRAM[address - 0x2000] = address % 64;
    }
}
char Mapper::getPPU(unsigned short address){
    return *getPPUPointerAt(address);
}
char* Mapper::getPPUPointerAt(unsigned short address){
    if(address >= 0x2000){
        return &VRAM[address - 0x2000];
    }
    return nullptr;
}

void Mapper::handlePPURegisterWrite(unsigned short address, char byte){
    PPU[address % 0x8] = byte;
    if(address == 0x2002){
        printf("[Mapper]\t clear VBlank\n");
        PPU[0x2] = PPU[0x2] & '\x7f'; // clear bit 7 (VBlank) on read.
    }
    if(address == 0x2005){
        PPUSCROLL = (PPUSCROLL << 8) + (byte & '\xff');
    }
    if(address == 0x2006){
        PPUADDR = ((PPUADDR & 0xff) << 8) + (unsigned char)byte;
        printf("[Mapper]\tNewPPUADDR: %x\n", PPUADDR);
    }
    if(address == 0x2007){
        printf("[Mapper]\tPPUADDR = %x, byte = %x\n", PPUADDR, byte);
        this->setPPU(PPUADDR, byte);
//        setPPU(PPUADDR, 1);
        if(PPU[0x0] & '\x04'){
            PPUADDR += 32;
        } else {
            PPUADDR += 1;
        }
    }
}

char Mapper::handlePPURegisterRead(unsigned short address){
    return PPU[address - 0x2000];
}


void Mapper::setVBlank(){
    PPU[2] |= '\x80'; //set bit 7
}

void Mapper::clearVBlank(){
    PPU[2] &= '\x7f'; // clear bit 7
}

unsigned char Mapper::getPPUSCROLLX(){
    return (PPUSCROLL >> 8) & '\xff';
}

unsigned char Mapper::getPPUSCROLLY(){
    return PPUSCROLL & '\xff';
}


