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

#include "Core/src/Core.hpp"
#include "Core/src/Mapper.hpp"
#include "Core/src/Mappers/Mapper000.hpp"
#include "Core/src/Mappers/Mapper001.hpp"

using namespace std;

int runNestest();
//int run_instr_test_v5();
static void hexDump(Core &cpu, unsigned short start, unsigned short end);
static int printable(char byte){return (((unsigned short)byte)&0xff);}


int main(int argc, const char * argv[]) {
    
    auto ret = runNestest();
//    auto ret = run_instr_test_v5();
    
    return ret;
    
//    return 0;
}

static void hexDump(Core &cpu, unsigned short start, unsigned short end) {
    for(int i = start; i < end; i+=0x10){
        printf("$%04x: ", i);
        for(int j = 0; j < 0x10; j++){
            printf("%02x ", printable(cpu.m->getByte(i + j)));
        }
        for(int j = 0; j < 0x10; j++){
            char c = cpu.m->getByte(i + j);
            if(c > 31 && c < 127){
                std::cout << c;
            } else {
                std::cout << '.';
            }
        }
        printf("\n");
    }
}

int run_instr_test_v5() {
    //PC = ea71
    
    Mapper001 map;
    
    ifstream file ("official_only.nes", ios::in|ios::binary|ios::ate);
    if(!file.is_open()){
        std::cout << "Unable to open file";
        return 1;
    }
    map.readINES(file);
    file.close();
    
    
    Core cpu = Core(&map);
    cpu.setPC(0xea71);


//    hexDump(cpu, 0xc000, 0xd000);

    char* output = cpu.m->getPointerAt(0x6000);

    *(output + 0) = 0;
    *(output + 1) = 0;
    *(output + 2) = 0;
    *(output + 3) = 0;
    *(output + 4) = 0;

    long instructionCount = 0;
    while(cpu.step() == 0){
        if(instructionCount % 100000 == 0){
            std::cout << "------" << instructionCount << "------" << std::endl;
            hexDump(cpu, 0x6000, 0x6100);
            instructionCount += 0;
        }
        instructionCount++;
    }


    hexDump(cpu, 0x6000, 0x7000);

    return 0;
}

int runNestest(){
    Mapper000 map;
    
    ifstream file ("nestest.nes", ios::in|ios::binary|ios::ate);
    if(!file.is_open()){
        std::cout << "Unable to open file";
        return 1;
    }
    map.readINES(file);
    
    Core cpu = Core(&map);
    cpu.setPC(0xc000);
    
    while(cpu.step() == 0);
    
    return 0;
}
