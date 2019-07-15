//
//  main.cpp
//  SoftDrop
//
//  Created by James Hovet on 7/14/19.
//  Copyright © 2019 James Hovet. All rights reserved.
//

#define private public // EVIL HACK

#include "main.h"
#include <iostream>
#include <fstream>
#include "../../Core/src/Core.hpp"

using namespace std;

int runNestest();

int main(int argc, const char * argv[]) {
    
    auto ret = runNestest();
    
    return ret;
}


int runNestest(){
    Core cpu;
    
    streampos totalSize;
    char * header;
    
    ifstream file ("nestest.nes", ios::in|ios::binary|ios::ate);
    if(!file.is_open()){
        std::cout << "Unable to open file";
        return 1;
    }
    
    totalSize = file.tellg();
    header = new char [16];
    totalSize -= 16;
    file.seekg (0, ios::beg);
    file.read (header, 16);
    
    //nestest.nes only
    
    char* PRG = &cpu.M[0];
    PRG += 0x8000;
    char* PRGMirror = PRG + 0x4000;
    file.read(PRG, 0x3fff);
    memcpy(PRGMirror, PRG, 0x3fff);
    
    file.close();
    
    //    cpu.setPC(0x85f5);
    cpu.setPC(0xc000);
    
    
    
    while(cpu.step() == 0);
    
    delete[] header;
    
    return 0;
}
