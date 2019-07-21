//
//  Mapper000.hpp
//  SoftDrop
//
//  Created by James Hovet on 7/17/19.
//  Copyright © 2019 James Hovet. All rights reserved.
//

#ifndef Mapper000_hpp
#define Mapper000_hpp

#include "Core/src/Mapper.hpp"

class Mapper000 : public Mapper {
private:
    char* memory;
    char header[0x10];
    
public:
    
    Mapper000();
    ~Mapper000();
    
    int readINES(std::ifstream &file);
    
    void setByte(unsigned short address, char byte);
    char getByte(unsigned short address);
    char* getPointerAt(unsigned short address);
    
    void setPPU(unsigned short address, char byte){}
    char getPPU(unsigned short address);
    char* getPPUPointerAt(unsigned short address);
};

#endif /* Mapper000_hpp */
