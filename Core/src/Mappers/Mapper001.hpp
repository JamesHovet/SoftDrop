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
    
public:
    
    Mapper001();
    ~Mapper001();
    
    int readINES(std::ifstream &file);
    
    void setByte(unsigned short address, char byte);
    char getByte(unsigned short address);
    char* getPointerAt(unsigned short address);
};

#endif /* Mapper001_hpp */
