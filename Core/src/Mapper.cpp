//
//  Mapper.cpp
//  SoftDrop
//
//  Created by James Hovet on 7/18/19.
//  Copyright © 2019 James Hovet. All rights reserved.
//
#include "Mapper.hpp"


/**
 send a message to set a given address to a given byte

 @param address the virtual address to set
 @param byte the byte to set it to
 */
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

/**
 send a message to read a byte from a given virtual address. Return that byte.

 @param address the address to read from
 @return the value at that virtual memory address.
 */
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

/**
 return a pointer to the real memory location of the given virtual memory address. Does not
 trigger any of the read-detection mechanisms (like those for the APUIO or PPU).

 @param address the desired virtual address
 @return pointer to the real memory location of the given virtual memory location
 */
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

/**
 Set a byte in VRAM/PPU RAM at a given address in VRAM

 @param address the VRAM address written to
 @param byte the byte to write
 */
void Mapper::setPPU(unsigned short address, char byte){
    if(address >= 0x2000){
        VRAM[(address % 0x4000) - 0x2000] = byte;
        //        VRAM[address - 0x2000] = address % 64;
    }
}

/**
 get a byte in VRAM/PPU RAM at a given VRAM address

 @param address the desired VRAM address
 @return the value at the given VRAM address
 */
char Mapper::getPPU(unsigned short address){
    return *getPPUPointerAt(address);
}

/**
 get a pointer to the real memory location of the given virtual VRAM memory location

 @param address the desired VRAM address
 @return pointer to the real location of the given virtual VRAM location
 */
char* Mapper::getPPUPointerAt(unsigned short address){
    if(address >= 0x2000){
        return &VRAM[address - 0x2000];
    }
    return nullptr;
}

/**
 Handle all necessary side-effects from writing to a PPU register (0x2000 - 0x2008, mirrored up
 to 0x3FFF). See https://wiki.nesdev.com/w/index.php/PPU_registers for more.

 @param address the virtual memory address to "write" to
 @param byte the byte to write
 */
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

/**
 return the value of a PPU register at the given address.

 @param address the address of the PPU register to read (must be 0x2000 - 0x2008)
 @return the value of the PPU register addressed at the given location
 */
char Mapper::handlePPURegisterRead(unsigned short address){
    return PPU[address - 0x2000];
}

/**
 Handle all the side effects of a write to one of the APUIO (Audio & IO) registers.

 @param address the address to write to (must be between 0x4000 and 0x4020)
 @param byte the byte to write.
 */
void Mapper::handleAPUIORegisterWrite(unsigned short address, char byte){
    if(address == 0x4016){
        tmpButtonValues = buttonValues;
        logf(Log::Level::Mapper | Log::Level::Controller, "Reset tmpButtonValues to %x\n", Utils::printable(buttonValues));
    }
    APUIO[address - 0x4000] = byte;
}

/**
 return the value at the APUIO register mapped at a given address and handle all side effects
 thereof (notably controller reading)

 @param address the location where the desired APUIO register is mapped (must be between
 0x4000 and 0x4020).
 @return the value of the APUIO register at the given location
 */
char Mapper::handleAPUIORegisterRead(unsigned short address){
    if(address == 0x4016){
        logf(Log::Level::Mapper | Log::Level::Controller, "tmpButtonValues:%x buttonValues:%x\n", Utils::printable(tmpButtonValues), Utils::printable(buttonValues));
        char val = tmpButtonValues & '\x01';
        tmpButtonValues = tmpButtonValues >> 1;
        return val;
    }
    return APUIO[address - 0x4000];
}

/**
 set the VBlank bit of the PPU high
 */
void Mapper::setVBlank(){
    PPU[2] |= '\x80'; //set bit 7
}

/**
 set the VBlank bit of the PPU high
 */
void Mapper::clearVBlank(){
    PPU[2] &= '\x7f'; // clear bit 7
}

/**
 return the X coordinate of the PPU scroll

 @return the X coordinate of the PPU scroll
 */
unsigned char Mapper::getPPUSCROLLX(){
    return (PPUSCROLL >> 8) & '\xff';
}

/**
 return the Y coordinate of the PPU scroll
 
 @return the Y coordinate of the PPU scroll
 */
unsigned char Mapper::getPPUSCROLLY(){
    return PPUSCROLL & '\xff';
}
