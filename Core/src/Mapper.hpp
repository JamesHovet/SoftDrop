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
public:
    
    Mapper(){}
    virtual ~Mapper(){};
    
    virtual int readINES(std::ifstream &openFile) = 0;
    
    virtual void setByte(unsigned short address, char byte) = 0;
    virtual char getByte(unsigned short address) = 0;
    virtual char* getPointerAt(unsigned short address) = 0;
};

#endif /* Mapper_h */
