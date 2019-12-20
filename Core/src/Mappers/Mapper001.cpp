//
//  Mapper001.cpp
//  SoftDrop
//
//  Created by James Hovet on 7/17/19.
//  Copyright © 2019 James Hovet. All rights reserved.
//

#include "Mapper001.hpp"

#include "Main/src/DebugOptions.hpp"
extern DebugOptions* debugOptions;

Mapper001::Mapper001(){
    memory = new char[0x0800];
}

Mapper001::~Mapper001(){
    delete[] memory;
    delete[] PRG;
    delete[] CHR;
    delete[] RAM;
}

int Mapper001::readINES(std::ifstream &file){
    Mapper::readINES(file);
    
    NumPRGBanks = header[4];
    PRG_ROM_C000_Bank = NumPRGBanks - 1; // initialize to
    NumCHRBanks = header[5];
    
    file.seekg(0x10, std::ios::beg);
    PRG = new char[0x4000 * NumPRGBanks];
    file.read(PRG, 0x4000 * NumPRGBanks);
    CHR = new char[0x2000 * NumCHRBanks];
    file.read(CHR, 0x2000 * NumCHRBanks);
    
    
    //TODO: check if RAM enabled
    RAM = new char[0x2000];
    
    return 0;
}

void Mapper001::setByte(unsigned short address, char byte){
    Mapper::setByte(address, byte);
    if(address >= 0x6000 && address < 0x8000){
        RAM[address - 0x6000] = byte;
    } else if (address >= 0x8000){
        if((byte & '\x80') == '\x80'){
            shiftRegister = 0x10; // reset shift register if bit 7 high
            shiftRegisterCount = 0;
        } else {
            shiftRegister = shiftRegister << 1;
            shiftRegister = shiftRegister | (byte & 0x1);
            shiftRegisterCount++;
            if(shiftRegisterCount == 5){
                unsigned char data = shiftRegister & 0x1F;
                shiftRegisterCount = 0;
                shiftRegister = 0x0;
                
                logf(Log::Level::Mapper | Log::Level::SubMapper,"Full Shift Register: %x@0x%4x\n", data, address);
                
                if(address <= 0x9FFF){
                    Register_Control = data;
                } else if(address <= 0xBFFF){
                    //TODO: Remove hack
//                    debugOptions->spritesheetOverride = data;
                    Register_CHR0 = data;
                    logf(Log::Level::Mapper | Log::Level::SubMapper,"Switching CHR0 Bank to %x\n", data);
                } else if(address <= 0xDFFF){
                    Register_CHR1 = data;
                    logf(Log::Level::Mapper | Log::Level::SubMapper,"Switching CHR1 Bank to %x\n", data);
                } else {
                    Register_PRG = data;
                    logf(Log::Level::Mapper | Log::Level::SubMapper,"Switching PRG Bank to %x\n", data);
                    switch ((Register_Control & 0xC) >> 2) {
                        case 0:
                        case 1:
                            //0, 1: switch 32 KB at $8000, ignoring low bit of bank number
                            PRG_ROM_8000_Bank = Register_PRG & 0x0e;
                            PRG_ROM_C000_Bank = (Register_PRG & 0x0e) + 1;
                            break;
                        case 2:
                            //2: fix first bank at $8000 and switch 16 KB bank at $C000
                            PRG_ROM_8000_Bank = 0;
                            PRG_ROM_C000_Bank = Register_PRG & 0x0f;
                            break;
                        case 3:
                            //3: fix last bank at $C000 and switch 16 KB bank at $8000
                            PRG_ROM_8000_Bank = Register_PRG & 0x0f;
                            PRG_ROM_C000_Bank = 1;
                            break;
                        default:
                            break;
                    }
                    //TODO: PRG RAM
                }
            }
        }
    }
}

inline char Mapper001::getByte(unsigned short address){
    return *getPointerAt(address);
}

inline char* Mapper001::getPointerAt(unsigned short address){
    char* superRes = Mapper::getPointerAt(address);
    if(superRes != nullptr){
        return superRes;
    }
    
    if(address >= 0x6000 && address < 0x8000){
        return &RAM[address - 0x6000];
    } else if(address < 0xC000){
        unsigned int offset = address - 0x8000;
        return PRG + offset + (0x4000 * PRG_ROM_8000_Bank);
    } else {
        unsigned int offset = address - 0xC000;
        return PRG + offset + (0x4000 * PRG_ROM_C000_Bank);
    }
}

//void Mapper001::setPPU(unsigned short address, char byte){
//    Mapper::setPPU(address, byte);
//}

char Mapper001::getPPU(unsigned short address){
    return *getPPUPointerAt(address);
}

char* Mapper001::getPPUPointerAt(unsigned short address){
    if(address < 0x1000){
        return &CHR[Register_CHR0 * 0x1000];
    } else if (address < 0x2000){
        return &CHR[Register_CHR1 * 0x1000];
    } else {
        return Mapper::getPPUPointerAt(address);
    }
    return nullptr;
}
