//
//  Mapper000.cpp
//  SoftDrop
//
//  Created by James Hovet on 7/17/19.
//  Copyright © 2019 James Hovet. All rights reserved.
//

#include "Mapper000.hpp"

Mapper000::Mapper000(){
    memory = new char[0x10000];
    CHR = new char[0x4000];
    
}

Mapper000::~Mapper000(){
    delete[] memory;
    delete[] CHR;
}

int Mapper000::readINES(std::ifstream &file){
    
    file.seekg(0, std::ios::beg);
    file.read(header, 0x10);
    
    char* PRG_ROM = &memory[0x8000];
    
    file.seekg(16, std::ios::beg); // after header
    file.read(PRG_ROM, 0x4000);
    //TODO: Actually check to see if Ines 2.0 header specifies size of PRG ROM;
    memcpy(PRG_ROM + 0x4000, PRG_ROM, 0x4000 * sizeof(char));
    
    file.read(CHR, 0x4000);
    
    return 0;
}

void Mapper000::setByte(unsigned short address, char byte){
    Mapper::setByte(address, byte);
    if(address < 0x2000){
        memory[address] = byte;
    }
}

inline char Mapper000::getByte(unsigned short address){
    return *getPointerAt(address);
}

inline char* Mapper000::getPointerAt(unsigned short address){
    char* superRes = Mapper::getPointerAt(address);
    if(superRes == nullptr){
        return &memory[address];
    } else {
        return superRes;
    }
}

inline char Mapper000::getPPU(unsigned short address){
    return *getPPUPointerAt(address);
}

inline char* Mapper000::getPPUPointerAt(unsigned short address){
    char* superRes = Mapper::getPPUPointerAt(address);
    if(superRes == nullptr){
        return &CHR[address];
    } else {
        return superRes;
    }
}
