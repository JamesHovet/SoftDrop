//
//  Mapper001.hpp
//  SoftDrop
//
//  Created by James Hovet on 7/17/19.
//  Copyright © 2019 James Hovet. All rights reserved.
//

#ifndef Mapper001_hpp
#define Mapper001_hpp

#include "Core/src/Mapper.hpp"

class Mapper001 : public Mapper {
private:
    char* memory;
    char* PRG;
    char* RAM;
    
    char PPU[0x8];
    
    unsigned char shiftRegister = 0x0;
    unsigned int shiftRegisterCount = 0;
    unsigned int NumPRGBanks = 0;
    unsigned int PRG_ROM_8000_Bank = 0;
    unsigned int PRG_ROM_C000_Bank = 1;
    
    unsigned int NumCHRBanks = 0;
    
    unsigned char Register_Control = 0x8;
    unsigned char Register_CHR0 = 0;
    unsigned char Register_CHR1 = 0;
    unsigned char Register_PRG = 0;
    
public:
    
    Mapper001();
    ~Mapper001();
    
    int readINES(std::ifstream &file);
    
    void setByte(unsigned short address, char byte);
    char getByte(unsigned short address);
    char* getPointerAt(unsigned short address);
    
//    void setPPU(unsigned short address, char byte);
    char getPPU(unsigned short address);
    char* getPPUPointerAt(unsigned short address);
};

#endif /* Mapper001_hpp */
