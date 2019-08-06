//
//  Mapper.hpp
//  SoftDrop
//
//  Created by James Hovet on 7/17/19.
//  Copyright © 2019 James Hovet. All rights reserved.
//

#ifndef Mapper_h
#define Mapper_h

#include <fstream>
#include <iostream>
#include "Utils/Utils.hpp"
#include "Utils/Logger.hpp"

class Mapper {
private:
    char APUIO[0x20];
    char PPU[0x8];
    char RAM[0x2000];
    char VRAM[0x2000];
    char OAM[0x100];
    
    unsigned short PPUADDR = 0;
    unsigned short PPUSCROLL = 0;
    
    char buttonValues;
    char tmpButtonValues;
    
public:
    Mapper(){}
    virtual ~Mapper(){};
    
    char* CHR;
    
    virtual int readINES(std::ifstream &openFile) = 0;
    char header[0x10];
    
    virtual void setByte(unsigned short address, char byte);
    virtual char getByte(unsigned short address);
    virtual char* getPointerAt(unsigned short address);
    
    virtual void setPPU(unsigned short address, char byte);
    virtual char getPPU(unsigned short address) = 0;
    virtual char* getPPUPointerAt(unsigned short address) = 0;
    
    char* getOAM(){return &OAM[0];}
    void setOAM(char* start){
        logf(Log::Level::Mapper, "OAM Copy\n");
        memcpy(OAM, start, sizeof(char) * 0x100);
    }
    
    void handlePPURegisterWrite(unsigned short address, char byte);
    char handlePPURegisterRead(unsigned short address);
    
    void handleAPUIORegisterWrite(unsigned short address, char byte);
    char handleAPUIORegisterRead(unsigned short address);
    
    void setVBlank();
    void clearVBlank();
    
    unsigned char getPPUSCROLLX();
    unsigned char getPPUSCROLLY();
    
    void setButtonValue(char value){buttonValues = value;}
    void orButtonValue(char value){buttonValues |= value;}
    
};

#endif /* Mapper_h */
