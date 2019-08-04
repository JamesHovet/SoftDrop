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

class Mapper {
private:
    char APUIO[0x20];
    char PPU[0x8];
    char RAM[0x2000];
    char VRAM[0x2000];
    char OAM[0x100];
public:
    Mapper(){}
    virtual ~Mapper(){};
    
    char* CHR;
    
    virtual int readINES(std::ifstream &openFile) = 0;
    char header[0x10];
    
    virtual void setByte(unsigned short address, char byte);
    virtual char getByte(unsigned short address);
    virtual char* getPointerAt(unsigned short address);
    
    virtual void setPPU(unsigned short address, char byte) = 0;
    virtual char getPPU(unsigned short address) = 0;
    virtual char* getPPUPointerAt(unsigned short address) = 0;
    
    char* getOAM(){return &OAM[0];}
    void setOAM(char* start){
        memcpy(OAM, start, sizeof(char) * 0x100);
    }
    
};

#endif /* Mapper_h */
