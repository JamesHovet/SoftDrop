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
    
    char header[0x10];
    bool inesMirrorBit = false;
    virtual int readINES(std::ifstream &file){
        file.seekg(0, std::ios::beg);
        file.read(header, 0x10);
        
        inesMirrorBit = Utils::getBitInByte(header[6],0);
        
        return 0;
    };
    
    
    virtual void setByte(unsigned short address, char byte);
    virtual char getByte(unsigned short address);
    virtual char* getPointerAt(unsigned short address);
    
    virtual void setPPU(unsigned short address, char byte);
    virtual char getPPU(unsigned short address) = 0;
    virtual char* getPPUPointerAt(unsigned short address) = 0;
    

    
    void handlePPURegisterWrite(unsigned short address, char byte);
    char handlePPURegisterRead(unsigned short address);
    
    void handleAPUIORegisterWrite(unsigned short address, char byte);
    char handleAPUIORegisterRead(unsigned short address);
    
    void setVBlank();
    void clearVBlank();
    
    //Register 2000: PPUCTRL
    unsigned char getBaseNametable(){return Utils::getShiftedValueOfBitRange(PPU[0], 0, 2);}
    bool getBackgroundPatternTableShift(){return Utils::getBitInByte(PPU[0], 4);}
    
    //Register 2001: PPUMASK
    bool shouldShowSprites(){return Utils::getBitInByte(PPU[1], 4);}
    bool shouldShowBackground(){return Utils::getBitInByte(PPU[1], 3);}
    //Register 2001: PPUSTATUS
    
    //Register 2003: OAMADDR
    
    //Register 2004 & 4014: OAM
    char* getOAM(){return &OAM[0];}
    void setOAM(char* start){
        logf(Log::Level::Mapper, "OAM Copy\n");
        memcpy(OAM, start, sizeof(char) * 0x100);
    }
    
    //Register 2005: PPUSCROLL
    unsigned char getPPUSCROLLX();
    unsigned char getPPUSCROLLY();
    
    //Register 2007: PPUDATA
    
    void setButtonValue(char value){buttonValues = value;}
    void orButtonValue(char value){buttonValues |= value;}
    
    friend void hexDumpCPU(Mapper& m, unsigned short start, unsigned short end);
    friend void hexDumpPPU(Mapper& m, unsigned short start, unsigned short end);
    friend void hexDumpOAM(Mapper& m);
    
};

#endif /* Mapper_h */
