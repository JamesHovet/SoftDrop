//
//  TestingMapper.cpp
//  SoftDrop
//
//  Created by James Hovet on 7/17/19.
//  Copyright © 2019 James Hovet. All rights reserved.
//

#include "TestingMapper.hpp"

TestingMapper::TestingMapper(){
    memory = new char[0x10000];
}

TestingMapper::~TestingMapper(){
    delete[] memory;
}

int TestingMapper::readINES(std::ifstream &file){
    return 0;
}

void TestingMapper::setByte(unsigned short address, char byte){
    memory[address] = byte;
}

char TestingMapper::getByte(unsigned short address){
    return memory[address];
}

char* TestingMapper::getPointerAt(unsigned short address){
    return &memory[address];
}
