//
//  TestingMapper.hpp
//  SoftDrop
//
//  Created by James Hovet on 7/17/19.
//  Copyright © 2019 James Hovet. All rights reserved.
//

#ifndef TestingMapper_hpp
#define TestingMapper_hpp

#include "Core/src/Mapper.hpp"

class TestingMapper : public Mapper {
private:
    char* memory;
    
public:
    TestingMapper();
    ~TestingMapper();
    
    int readINES(std::ifstream &file);
    
    void setByte(unsigned short address, char byte);
    char getByte(unsigned short address);
    char* getPointerAt(unsigned short address);
};

#endif /* TestingMapper_hpp */
