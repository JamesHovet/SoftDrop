//
//  Core.hpp
//  jNES
//
//  Created by James Hovet on 7/11/19.
//  Copyright © 2019 James Hovet. All rights reserved.
//

#ifndef Core_hpp
#define Core_hpp

#include <iostream>
#include <sstream>
#include <stdio.h>
#include <cstring>
#include "Core/src/Mapper.hpp"
#include "Utils/Utils.hpp"

#define DEFAULT_FLAGS '\x20'
#define ZERO_FLAG '\x02'
#define NEGATIVE_FLAG '\x80'
#define CARRY_FLAG '\x01'
#define OVERFLOW_FLAG '\x40'
#define INTERRUPT_FLAG '\x04'
#define DECIMAL_FLAG '\x08'

#define PAGE 0x100
#define STACK_TOP 255
#define PAGE_ONE 0x100
#define RESET_VECTOR_LOCATION 0xFFFC
#define NMI_VECTOR_LOCATION 0xFFFA
#define IRQ_VECTOR_LOCATION 0xFFFE

#define TESTING_PRG_OFFSET 0x8000

class Core{
public:

    Core(Mapper& mapper);
    ~Core();
    
    enum AddressMode {
        special,           // 0
        indexedIndirect,   // 1
        immediete,         // 2
        zeroPage,          // 3
        implicit,          // 4
        absolute,          // 5
        offset,            // 6
        indirectIndexed,   // 7
        zeroPageX,         // 8
        zeroPageY,         // 9
        absoluteY,         // 10
        absoluteX,         // 11
        indirect,          // 12 - just 0x6C - JMP (a)
        acculumlator       // 13
    };
    
    enum Flag {
        zero = ZERO_FLAG,
        negative = NEGATIVE_FLAG,
        carry = CARRY_FLAG,
        overflow = OVERFLOW_FLAG,
        interrupt = INTERRUPT_FLAG,
        decimal = DECIMAL_FLAG
    };
    
private:
    char A;
    char X;
    char Y;
    unsigned char SP = STACK_TOP;
    char flags = DEFAULT_FLAGS;//NV-BDIZC; the 3rd most significant bit is always 1
    unsigned short PC;
    unsigned long clock = 7;
    bool IRQ = false;
    bool shouldIRQAfterRTI = false;
    bool NMI = false;
    
    Mapper& m;
    
    // Abstractions
    unsigned short getAddress(AddressMode mode);
    unsigned short getAddress(AddressMode mode, bool shouldCheckPageOverflow);
    char inline getByte(AddressMode mode){return m.getByte(getAddress(mode));}
    char inline getByte(AddressMode mode, bool shouldCheckPageOverflow) {
        return m.getByte(getAddress(mode, shouldCheckPageOverflow));
    }

    void loadRegister(char byte, char* reg);
    void inline loadRegister(AddressMode mode, char* reg){loadRegister(getByte(mode, true), reg);}
    void storeRegister(char byte, unsigned short address){
        if(address == 0x4014){
            OAMCopy(byte);
        } else {
            m.setByte(address, byte);
        }
    }
    void inline storeRegister(AddressMode mode, char byte){storeRegister(byte, getAddress(mode));}
    void increment(char* reg);
    void inline increment(AddressMode mode){increment(m.getPointerAt(getAddress(mode)));}
    void decrement(char* reg);
    void inline decrement(AddressMode mode){decrement(m.getPointerAt(getAddress(mode)));}
    void addWithCarry(char byte);
    void inline addWithCarry(AddressMode mode){addWithCarry(getByte(mode, true));}
    void subWithCarry(char byte);
    void inline subWithCarry(AddressMode mode){subWithCarry(getByte(mode, true));}
    void shiftLeft(char* byte);
    void shiftLeft(AddressMode mode);
    void shiftRight(char* byte);
    void shiftRight(AddressMode mode);
    void rotateLeft(char* byte);
    void rotateLeft(AddressMode mode);
    void rotateRight(char* byte);
    void rotateRight(AddressMode mode);
    void compare(char byte, char reg);
    void inline compare(AddressMode mode, char reg){compare(getByte(mode, true), reg);}
    void bit(char byte);
    void inline bit(AddressMode mode){bit(getByte(mode));}
    void jump(unsigned short address){PC = address;}
    void inline jump(AddressMode mode){jump(getAddress(mode));}
    void branch(AddressMode mode);
    void branchConditional(AddressMode mode, bool condition);
    void push(char byte){m.setByte(PAGE_ONE + SP--, byte);}
    void pull(char* dest){*dest = m.getByte(PAGE_ONE + ++SP);}
    unsigned short pullAddress();
    void reset();
    
    void OAMCopy(char byte){
        m.setOAM(m.getPointerAt(((unsigned short)byte) << 8));
        if(clock % 2 == 1){
            clock += 513;
        }else{
            clock += 514;
        }
    }
    
    // Utils
    void setFlag(Flag flag, bool val);
    void setArithmaticFlags(char byte);
    AddressMode getAddressModeFromOpcode(char opcode);
    inline bool checkPageOverflow(unsigned short a1, unsigned short a2){
        return (a1 / 0x100) != (a2 / 0x100);
    }
    inline void handleCheckPageOverflow(unsigned short a1, unsigned short a2){
        if(checkPageOverflow(a1, a2)){
            clock += 1;
//            std::cout << "\t+1";
        };
    }
    unsigned short getIndirectWithWrapping(unsigned short address);
    void debugPrintCPU();

    
public:
    int stepTo(unsigned long clockCycle);
    int step();
    unsigned long getClock(){return clock;}
    inline void setIRQ(){IRQ = true;}
    inline void setNMI(){NMI = true;}
    void loadIntoMemory(char* bytes, unsigned short address, unsigned short length);
    void loadIntoMemory(std::string hex_chars, unsigned short address);
    void setPC(unsigned short address){PC = address;}
    void loadProgram(std::string hex_chars){
        loadIntoMemory(hex_chars, TESTING_PRG_OFFSET);
        setPC(TESTING_PRG_OFFSET);
        clock = 0;
    }
    void runProgram(std::string hex_chars){
        std::cout << "running program\n" << hex_chars << std::endl;
        loadProgram(hex_chars);
        flags = DEFAULT_FLAGS;
        A = 0;
        X = 0;
        Y = 0;
        SP = STACK_TOP;
        while(step() == 0);
        printf("clock: %lu\n\n", clock);
    }
    bool getFlag(Flag flag){return (flags & flag) == flag;}
    
};



void step();





#endif /* Core_hpp */
