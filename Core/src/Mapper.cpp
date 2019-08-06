//
//  Mapper.cpp
//  SoftDrop
//
//  Created by James Hovet on 7/18/19.
//  Copyright © 2019 James Hovet. All rights reserved.
//
#include "Mapper.hpp"


void Mapper::setByte(unsigned short address, char byte){
    if(address < 0x2000){
        RAM[address] = byte;
    } else if(address >= 0x4000 && address < 0x4020){
        logf(Log::Level::Mapper, "Wrote %x into APUIO address 0x%x\n", byte, address);
        handleAPUIORegisterWrite(address, byte);
    } else if(address >= 0x2000 && address < 0x4000){
        PPU[address % 0x8] = byte;
        logf(Log::Level::Mapper, "Write %x to PPU Register %x\n", Utils::printable(byte), address);
        handlePPURegisterWrite(address, byte);
    }
}

char Mapper::getByte(unsigned short address){
    if(address >= 0x4000 && address < 0x4020){
        return handleAPUIORegisterRead(address);
    }
    else if(address >= 0x2000 && address < 0x4000){
        address = 0x2000 + (address % 0x8);
        char res = handlePPURegisterRead(address);
        logf(Log::Level::Mapper, "Read %x from PPU Register %x\n", Utils::printable(res), address);
        return res;
    }
    
    return *getPointerAt(address);
}

char* Mapper::getPointerAt(unsigned short address){
    if(address < 0x2000){
        return &RAM[address];
    }else if(address >= 0x4000 && address < 0x4020){
        logf(Log::Level::Mapper, "Read* APUIO address 0x%x\n", address);
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
        logf(Log::Level::Mapper, " clear VBlank\n");
        PPU[0x2] = PPU[0x2] & '\x7f'; // clear bit 7 (VBlank) on read.
    }
    if(address == 0x2005){
        PPUSCROLL = (PPUSCROLL << 8) + (byte & '\xff');
    }
    if(address == 0x2006){
        PPUADDR = ((PPUADDR & 0xff) << 8) + (unsigned char)byte;
        logf(Log::Level::Mapper, "NewPPUADDR: %x\n", PPUADDR);
    }
    if(address == 0x2007){
        logf(Log::Level::Mapper, "PPUADDR = %x, byte = %x\n", PPUADDR, byte);
        this->setPPU(PPUADDR, byte);
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

void Mapper::handleAPUIORegisterWrite(unsigned short address, char byte){
    if(address == 0x4016){
        tmpButtonValues = buttonValues;
        logf(Log::Level::Mapper | Log::Level::Controller, "Reset tmpButtonValues to %x\n", Utils::printable(buttonValues));
    }
    APUIO[address - 0x4000] = byte;
}

char Mapper::handleAPUIORegisterRead(unsigned short address){
    if(address == 0x4016){
        logf(Log::Level::Mapper | Log::Level::Controller, "tmpButtonValues:%x buttonValues:%x\n", Utils::printable(tmpButtonValues), Utils::printable(buttonValues));
        char val = tmpButtonValues & '\x01';
        tmpButtonValues = tmpButtonValues >> 1;
        return val;
    }
    return APUIO[address - 0x4000];
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
