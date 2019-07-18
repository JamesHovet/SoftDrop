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
public:
    
    Mapper(){}
    virtual ~Mapper(){};
    
    virtual int readINES(std::ifstream &openFile) = 0;
    
    virtual void setByte(unsigned short address, char byte);
    virtual char getByte(unsigned short address);
    virtual char* getPointerAt(unsigned short address);
};

#endif /* Mapper_h */
