//
//  Mapper001.cpp
//  SoftDrop
//
//  Created by James Hovet on 7/17/19.
//  Copyright © 2019 James Hovet. All rights reserved.
//

#include "Mapper001.hpp"

Mapper001::Mapper001(){
    memory = new char[0x10000];
}

Mapper001::~Mapper001(){
    delete[] memory;
}

int Mapper001::readINES(std::ifstream &file){
    
    char* PRG_ROM = &memory[0x8000];
    
    file.seekg(16, std::ios::beg); // after header
    file.read(PRG_ROM, 0x4000);
    //TODO: Actually check to see if Ines 2.0 header specifies size of PRG ROM;
    memcpy(PRG_ROM + 0x4000, PRG_ROM, 0x4000 * sizeof(char));
//    file.close();
    
    return 0;
}

void Mapper001::setByte(unsigned short address, char byte){
    if(address < 0x2000){
        memory[address] = byte;
    }
}

inline char Mapper001::getByte(unsigned short address){
    return memory[address];
}

inline char* Mapper001::getPointerAt(unsigned short address){
    return &memory[address];
}
