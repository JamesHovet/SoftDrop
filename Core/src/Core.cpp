//
//  core.cpp
//  jNES
//
//  Created by James Hovet on 7/11/19.
//  Copyright © 2019 James Hovet. All rights reserved.
//

//#define CUSTOM_DEBUG

#include "Core.hpp"
const int cycleCounts[0x100] = {
    7,6,0,0,0,3,5,0,3,2,2,0,0,4,6,0,2,5,0,0,0,4,6,0,2,4,0,0,0,4,7,0,6,6,0,0,3,3,5,0,4,2,2,0,4,4,6,0,2,5,0,0,0,4,6,0,2,4,0,0,0,4,7,0,6,6,0,0,0,3,5,0,3,2,2,0,3,4,6,0,2,5,0,0,0,4,6,0,2,4,0,0,0,4,7,0,6,6,0,0,0,3,5,0,4,2,2,0,5,4,6,0,2,5,0,0,0,4,6,0,2,4,0,0,0,4,7,0,0,6,0,0,3,3,3,0,2,0,2,0,4,4,4,0,2,6,0,0,4,4,4,0,2,5,2,0,0,5,0,0,2,6,2,0,3,3,3,0,2,2,2,0,4,4,4,0,2,5,0,0,4,4,4,0,2,4,2,0,4,4,4,0,2,6,0,0,3,3,5,0,2,2,2,0,4,4,6,0,2,5,0,0,0,4,6,0,2,4,0,0,0,4,7,0,2,6,0,0,3,3,5,0,2,2,2,0,4,4,6,0,2,5,0,0,0,4,6,0,2,4,0,0,0,4,7,0
    
};
const Core::AddressMode addressModeMap[0x20] = {
    Core::AddressMode::special,
    Core::AddressMode::indexedIndirect,
    Core::AddressMode::immediete,
    Core::AddressMode::indexedIndirect,
    Core::AddressMode::zeroPage,
    Core::AddressMode::zeroPage,
    Core::AddressMode::zeroPage,
    Core::AddressMode::zeroPage,
    Core::AddressMode::implicit,
    Core::AddressMode::immediete,
    Core::AddressMode::acculumlator,
    Core::AddressMode::immediete,
    Core::AddressMode::special,
    Core::AddressMode::absolute,
    Core::AddressMode::absolute,
    Core::AddressMode::absolute,
    Core::AddressMode::offset,
    Core::AddressMode::indirectIndexed,
    Core::AddressMode::implicit,
    Core::AddressMode::indirectIndexed,
    Core::AddressMode::zeroPageX,
    Core::AddressMode::zeroPageX,
    Core::AddressMode::special,
    Core::AddressMode::special,
    Core::AddressMode::implicit,
    Core::AddressMode::absoluteY,
    Core::AddressMode::implicit,
    Core::AddressMode::absoluteY,
    Core::AddressMode::absoluteX,
    Core::AddressMode::absoluteX,
    Core::AddressMode::special,
    Core::AddressMode::special
};

// debug
const std::string debugOpNames[] = {
    "BRK","ORA","STP","SLO","NOP","ORA","ASL","SLO","PHP","ORA","ASL","ANC","NOP","ORA","ASL","SLO","BPL","ORA","STP","SLO","NOP","ORA","ASL","SLO","CLC","ORA","NOP","SLO","NOP","ORA","ASL","SLO","JSR","AND","STP","RLA","BIT","AND","ROL","RLA","PLP","AND","ROL","ANC","BIT","AND","ROL","RLA","BMI","AND","STP","RLA","NOP","AND","ROL","RLA","SEC","AND","NOP","RLA","NOP","AND","ROL","RLA","RTI","EOR","STP","SRE","NOP","EOR","LSR","SRE","PHA","EOR","LSR","ALR","JMP","EOR","LSR","SRE","BVC","EOR","STP","SRE","NOP","EOR","LSR","SRE","CLI","EOR","NOP","SRE","NOP","EOR","LSR","SRE","RTS","ADC","STP","RRA","NOP","ADC","ROR","RRA","PLA","ADC","ROR","ARR","JMP","ADC","ROR","RRA","BVS","ADC","STP","RRA","NOP","ADC","ROR","RRA","SEI","ADC","NOP","RRA","NOP","ADC","ROR","RRA","NOP","STA","NOP","SAX","STY","STA","STX","SAX","DEY","NOP","TXA","XAA","STY","STA","STX","SAX","BCC","STA","STP","AHX","STY","STA","STX","SAX","TYA","STA","TXS","TAS","SHY","STA","SHX","AHX","LDY","LDA","LDX","LAX","LDY","LDA","LDX","LAX","TAY","LDA","TAX","LAX","LDY","LDA","LDX","LAX","BCS","LDA","STP","LAX","LDY","LDA","LDX","LAX","CLV","LDA","TSX","LAS","LDY","LDA","LDX","LAX","CPY","CMP","NOP","DCP","CPY","CMP","DEC","DCP","INY","CMP","DEX","AXS","CPY","CMP","DEC","DCP","BNE","CMP","STP","DCP","NOP","CMP","DEC","DCP","CLD","CMP","NOP","DCP","NOP","CMP","DEC","DCP","CPX","SBC","NOP","ISC","CPX","SBC","INC","ISC","INX","SBC","NOP","SBC","CPX","SBC","INC","ISC","BEQ","SBC","STP","ISC","NOP","SBC","INC","ISC","SED","SBC","NOP","ISC","NOP","SBC","INC","ISC"
    
};

inline static unsigned short promoteUnsigned(char byte){
    return (((unsigned short)byte)&0xff);
}

inline static signed short promoteSigned(char byte){
    return (((signed short)byte));
}

/**
 Initialize a virtual cpu with a reference to a given memory mapper

 @param mapper a memory mapper to handle all memory access for this virtual cpu
 */
Core::Core(Mapper& mapper) : m(mapper) {
    
}

Core::~Core(){
    
}


/**
 perform a memcpy from a given location to a given address in virtual memory of a given length.
 Be careful when calling this method because the only memory location that is garunteed to be
 correct is the 0th, all others are liable to fail. Reccomended for debugging only.

 @param bytes pointer to first byte to copy
 @param address virtual memory address to copy to
 @param length the length, in bytes, of memory to copy
 */
void Core::loadIntoMemory(char* bytes, unsigned short address, unsigned short length){
    memcpy(m.getPointerAt(address), bytes, length);
}

/**
 Copy a string of hex bytes represented as ascii characters into a given space in virtual memory.
 Reccomended only for testing/debugging small sets of 6502 code.

 @param hex_chars a string of ascii representing hex bytes. ex. "a9 20 85 01 65 01 00"
 @param address virtual memory address to copy to
 */
void Core::loadIntoMemory(std::string hex_chars, unsigned short address){
    std::istringstream hex_chars_stream(hex_chars);
    
    unsigned int c;
    while (hex_chars_stream >> std::hex >> c)
    {
        m.setByte(address++, (char)c);
    }
}

/**
 process instructions until the internal clock has advanced past a given clock cycle. This
 method is garunteed to reach the passed clock cycle, but is liable to proceed at most one
 "step" or instruction beyond.

 @param clockCycle the clock cycle to reach before stopping
 @return the status code of the virtual cpu after the last step (non-zero if an error code)
 */
int Core::stepTo(unsigned long clockCycle){
    while(clock < clockCycle){
        int status = step();
        if(status != 0){
            return status;
        }
    }
    return 0;
}

// Processor


/**
 Process one processor instruction and, if applicable, service an interrupt first. Automatically
 increments the processor clock. Return one of the status codes listed below (non-zero if error)
 
 0: No error
 1: CPU hit a BRK instruction

 @return the status code of the virtual cpu after the last step (non-zero if an error code)
 */
int Core::step(){
    //handle interrupts
    if(NMI || IRQ){
        push((char)((PC >> 8) & '\xff'));
        push((char)(PC & '\xff'));
        push(flags);
        if(NMI){
            logf(Log::Level::Core, "Servicing NMI; Current PC: %x\n", PC);
            PC = getIndirectWithWrapping(NMI_VECTOR_LOCATION);
            NMI = false;
            if(IRQ){
                IRQ = false;
                shouldIRQAfterRTI = true; // if both are set, the IRQ should be handled after the RTI
            }
        } else if(IRQ){
            printf("[Core]\tServicing IRQ; Current PC: %x\n", PC);
            PC = getIndirectWithWrapping(IRQ_VECTOR_LOCATION);
            IRQ = false;
        }
        clock += 7;
        return 0;
    }
    
    //setup main opcode handling
    char op = m.getByte(PC++);
    unsigned char op_u = (unsigned char)op;
    AddressMode mode = getAddressModeFromOpcode(op);
    
    //debug
#ifdef CUSTOM_DEBUG
    
    printf("[Core]\t");
    printf("$%04x:%02x", PC - 1, Utils::printable(op));
    printf("\t");
    printf("A:%2x\tX:%2x\tY:%2x\tSP:%2x\tf:%2x\t",
           Utils::printable(A),
           Utils::printable(X),
           Utils::printable(Y),
           Utils::printable(SP),
           Utils::printable(flags));
    
    std::cout << debugOpNames[op_u];
    
    for(int i = 0; i < debugOpBytes[op_u] - 1; i++){
        std::cout << " " << std::hex << Utils::printable(m.getByte(PC + i));
    }
    
    printf("\tCYC:%lu", clock);
    printf("\tM:%d", mode);
    
    std::cout << std::endl;
#endif
    
    // increment the clock
    clock += cycleCounts[op_u];
    
    
    //main opcodes
    
    /* LDA */
    switch (op) {
            
            /* LDA */
        case '\xa1':
        case '\xa5':
        case '\xa9':
        case '\xad':
        case '\xb1':
        case '\xb5':
        case '\xb9':
        case '\xbd':
            loadRegister(mode, &A);
            return 0;
            break;
            
            /* LDX */
        case '\xa2':
        case '\xa6':
        case '\xae':
        case '\xb6':
        case '\xbe':
            loadRegister(mode, &X);
            return 0;
            break;
            
            /* LDY */
        case '\xa0':
        case '\xa4':
        case '\xac':
        case '\xb4':
        case '\xbc':
            loadRegister(mode, &Y);
            return 0;
            break;
            
            /* STA */
        case '\x81':
        case '\x85':
        case '\x8d':
        case '\x91':
        case '\x95':
        case '\x99':
        case '\x9d':
            storeRegister(mode, A);
            return 0;
            break;
            
            /* STX */
        case '\x86':
        case '\x96':
        case '\x8e':
            storeRegister(mode, X);
            return 0;
            break;
            
            /* STY */
        case '\x84':
        case '\x94':
        case '\x8c':
            storeRegister(mode, Y);
            return 0;
            break;

            /* PHA */
        case '\x48':
            push(A);
            return 0;
            break;
            
            /* PLA */
        case '\x68':
            pull(&A);
            setArithmaticFlags(A);
            return 0;
            break;
            
            /* PHP */
        case '\x08':
            push(flags);
            return 0;
            break;
            
            /* PLP */
        case '\x28':
            pull(&flags);
            return 0;
            break;
            
            /* TXS */
        case '\x9a':
            SP = (unsigned char)X;
            return 0;
            break;
            
            /* TSX */
        case '\xba':
            X = (signed char)SP;
            setArithmaticFlags(X);
            return 0;
            break;
            
            /* INX */
        case '\xe8':
            increment(&X);
            return 0;
            break;
            
            /* INY */
        case '\xc8':
            increment(&Y);
            return 0;
            break;
            
            /* DEX */
        case '\xca':
            decrement(&X);
            return 0;
            break;
            
            /* DEY */
        case '\x88':
            decrement(&Y);
            return 0;
            break;
            
            /* INC */
        case '\xE6':
        case '\xF6':
        case '\xEE':
        case '\xFE':
            increment(mode);
            return 0;
            break;
            
            /* DEC */
        case '\xC6':
        case '\xD6':
        case '\xCE':
        case '\xDE':
            decrement(mode);
            return 0;
            break;
            
            /* ADC */
        case '\x69':
        case '\x65':
        case '\x75':
        case '\x6D':
        case '\x7D':
        case '\x79':
        case '\x61':
        case '\x71':
            addWithCarry(mode);
            return 0;
            break;
            
            /* SBC */
        case '\xE9':
        case '\xE5':
        case '\xF5':
        case '\xED':
        case '\xFD':
        case '\xF9':
        case '\xE1':
        case '\xF1':
            subWithCarry(mode);
            return 0;
            break;
            
            /* ASL */
        case '\x0A':
        case '\x06':
        case '\x16':
        case '\x0E':
        case '\x1E':
            shiftLeft(mode);
            return 0;
            break;
            
            /* LSR */
        case '\x4A':
        case '\x46':
        case '\x56':
        case '\x4E':
        case '\x5E':
            shiftRight(mode);
            return 0;
            break;
            
            /* ROL */
        case '\x2A':
        case '\x26':
        case '\x36':
        case '\x2E':
        case '\x3E':
            rotateLeft(mode);
            return 0;
            break;
            
            /* ROR */
        case '\x6A':
        case '\x66':
        case '\x76':
        case '\x6E':
        case '\x7E':
            rotateRight(mode);
            return 0;
            break;
            
            /* AND */
        case '\x29':
        case '\x25':
        case '\x35':
        case '\x2D':
        case '\x3D':
        case '\x39':
        case '\x21':
        case '\x31':
            A = A & getByte(mode, true);
            setArithmaticFlags(A);
            return 0;
            break;
            
            /* EOR */
        case '\x49':
        case '\x45':
        case '\x55':
        case '\x4D':
        case '\x5D':
        case '\x59':
        case '\x41':
        case '\x51':
            A = A ^ getByte(mode, true);
            setArithmaticFlags(A);
            return 0;
            break;
            
            /* ORA */
        case '\x09':
        case '\x05':
        case '\x15':
        case '\x0D':
        case '\x1D':
        case '\x19':
        case '\x01':
        case '\x11':
            A = A | getByte(mode, true);
            setArithmaticFlags(A);
            return 0;
            break;
            
            /* CMP */
        case '\xC9':
        case '\xC5':
        case '\xD5':
        case '\xCD':
        case '\xDD':
        case '\xD9':
        case '\xC1':
        case '\xD1':
            compare(mode, A);
            return 0;
            break;
            
            /* CPX */
        case '\xe0':
        case '\xe4':
        case '\xec':
            compare(mode, X);
            return 0;
            break;
            
            /* CPY */
        case '\xc0':
        case '\xc4':
        case '\xcc':
            compare(mode, Y);
            return 0;
            break;
            
            /* BIT */
        case '\x24':
        case '\x2c':
            bit(mode);
            return 0;
            break;
            
            /* BCC */
        case '\x90':
            branchConditional(mode, !getFlag(Flag::carry));
            return 0;
            break;
            
            /* BCS */
        case '\xB0':
            branchConditional(mode, getFlag(Flag::carry));
            return 0;
            break;
            
            /* BEQ */
        case '\xf0':
            branchConditional(mode, getFlag(Flag::zero));
            return 0;
            break;
            
            /* BNE */
        case '\xd0':
            branchConditional(mode, !getFlag(Flag::zero));
            return 0;
            break;
            
            /* BMI */
        case '\x30':
            branchConditional(mode, getFlag(Flag::negative));
            return 0;
            break;
            
            /* BPL */
        case '\x10':
            branchConditional(mode, !getFlag(Flag::negative));
            return 0;
            break;
            
            /* BVC */
        case '\x50':
            branchConditional(mode, !getFlag(Flag::overflow));
            return 0;
            break;
            
            /* BVS */
        case '\x70':
            branchConditional(mode, getFlag(Flag::overflow));
            return 0;
            break;
            
            /* JMP */
        case '\x4c':
        case '\x6c':
            jump(mode);
            return 0;
            break;
            
            /* SEC */
        case '\x38':
            setFlag(Flag::carry, true);
            return 0;
            break;
            
            /* CLC */
        case '\x18':
            setFlag(Flag::carry, false);
            return 0;
            break;
        
            /* CLI */
        case '\x58':
            setFlag(Flag::interrupt, false);
            return 0;
            break;
            
            /* SEI */
        case '\x78':
            setFlag(Flag::interrupt, true);
            return 0;
            break;
            
            /* CLV */
        case '\xb8':
            setFlag(Flag::overflow, false);
            return 0;
            break;
            
            /* SED */
        case '\xf8':
            setFlag(Flag::decimal, true);
            return 0;
            break;
            
            /* CLD */
        case '\xd8':
            setFlag(Flag::decimal, false);
            return 0;
            break;
            
            /* TAY */
        case '\xa8':
            Y = A;
            setArithmaticFlags(Y);
            return 0;
            break;
            
            /* TYA */
        case '\x98':
            A = Y;
            setArithmaticFlags(A);
            return 0;
            break;
            
            /* TXA */
        case '\x8a':
            A = X;
            setArithmaticFlags(A);
            return 0;
            break;
            
            /* TAX */
        case '\xaa':
            X = A;
            setArithmaticFlags(X);
            return 0;
            break;
            
            /* NOP */
        case '\xea':
            return 0;
            break;
            
            /* JSR */
        case '\x20':
            push((unsigned char)((PC + 1) >> 8));
            push((unsigned char)((PC + 1) & 0xff));
            jump(getAddress(mode));
            return 0;
            break;
            
            /* RTS */
        case '\x60':
            jump(pullAddress() + 1);
            return 0;
            break;
            
            /* RTI */
        case '\x40':
            pull(&flags);
            jump(pullAddress());
            if(shouldIRQAfterRTI){IRQ = true;}
            return 0;
            break;
            
            /* BRK */
        case '\x00':
            return 1;
            break;
        
        default:
//            std::cout << "Invalid opcode: " << std::hex << (((unsigned short)op) & 0x00ff) << std::endl;
            break;
    }
    return 0;
}


// Abstrations

/**
 get the virtual address needed by the current opcode based on the addressing mode of that
 opcode. For example, if the current opcode needs to know a given value in zero-page memory,
 return the virtual address of that memory location by processing the arguement of that opcode
 and increase the PC.
 
 Changes: PC

 @param mode the AddressMode of the opcode to be processed
 @return the vitual memory address needed by the opcode to be processed.
 */
unsigned short Core::getAddress(AddressMode mode){
    unsigned short address = PC + 1;
    switch (mode) {
        case AddressMode::immediete:
            return PC++;
        case AddressMode::offset:
            return PC++;
        case AddressMode::zeroPage:
            return promoteUnsigned(m.getByte(PC++));
        case AddressMode::zeroPageX:
            return (promoteUnsigned(m.getByte(PC++)) + promoteUnsigned(X)) % 0x100;
        case AddressMode::zeroPageY:
            return (promoteUnsigned(m.getByte(PC++)) + promoteUnsigned(Y)) % 0x100;
        case AddressMode::absolute:
            address = promoteUnsigned(m.getByte(PC)) + (promoteUnsigned(m.getByte(PC + 1)) << 8);
            PC += 2;
            return address; // return early to avoid triggering the page boundary code below
        case AddressMode::absoluteX:
            address = promoteUnsigned(m.getByte(PC)) + (promoteUnsigned(m.getByte(PC + 1)) << 8);
            address += promoteUnsigned(X);
            PC += 2;
            break;
        case AddressMode::absoluteY:
            address = promoteUnsigned(m.getByte(PC)) + (promoteUnsigned(m.getByte(PC + 1)) << 8);
            address += promoteUnsigned(Y);
            PC += 2;
            break;
        case AddressMode::indirect:
            address = promoteUnsigned(m.getByte(PC)) + (promoteUnsigned(m.getByte(PC + 1)) << 8);
            address = getIndirectWithWrapping(address);
            PC +=2;
            break;
        case AddressMode::indexedIndirect:
            address = getIndirectWithWrapping(promoteUnsigned(m.getByte(PC)) + X);
            PC++;
            break;
        case AddressMode::indirectIndexed:
            address = getIndirectWithWrapping(promoteUnsigned(m.getByte(PC))) + promoteUnsigned(Y);
            PC++;
            break;
        default:
            break;
    }
    
    return address;
}

/**
 get the virtual address needed by the current opcode based on the addressing mode of that
 opcode (see Core::getAddress for more.) If shouldCheckPageOverflow is true, also increments
 the clock if a memory page boundary is crossed.
 
 Changes: PC, clock

 @param mode the AddressMode of the opcode to be processed
 @param shouldCheckPageOverflow true if this method should check for a page boundary crossing
 @return the virtual memory address needed by the opcode to be processed.
 */
unsigned short Core::getAddress(AddressMode mode, bool shouldCheckPageOverflow){
    if(shouldCheckPageOverflow &&
       (mode == absoluteX || mode == absoluteY || mode == indirectIndexed)
       ){
        unsigned short a1 = 0;
        unsigned short a2 = 0;
        if(mode == absoluteX){
            a1 = promoteUnsigned(m.getByte(PC)) + (promoteUnsigned(m.getByte(PC + 1)) << 8);
            a2 = a1 + promoteUnsigned(X);
            PC += 2;
        } else if (mode == absoluteY){
            a1 = promoteUnsigned(m.getByte(PC)) + (promoteUnsigned(m.getByte(PC + 1)) << 8);
            a2 = a1 + promoteUnsigned(Y);
            PC += 2;
        } else if (mode == indirectIndexed){
            a1 = getIndirectWithWrapping(promoteUnsigned(m.getByte(PC)));
            a2 = a1 + promoteUnsigned(Y);
            PC++;
        }
        handleCheckPageOverflow(a1, a2);
        return a2;
    } else {
        return getAddress(mode);
    }
}


/**
 load a byte into a given virtual register and set associated arithmatic flags.
 
 Changes: registers, flags

 @param byte the byte to be loaded in
 @param reg a pointer to the register to be loaded into
 */
void Core::loadRegister(char byte, char* reg){
    *reg = byte;
    setArithmaticFlags(byte);
}

/**
 Increment a given byte of memory (register or RAM) and set associated arithmatic flags.
 
 Changes: memory or registers, flags

 @param byte pointer to the byte to be incremented
 */
void Core::increment(char* byte){
    *byte = *byte + 1;
    setArithmaticFlags(*byte);
}
/**
 Decrement a given byte of memory (register or RAM) and set associated arithmatic flags.
 
 Changes: memory or registers, flags
 
 @param byte pointer to the byte to be incremented
 */
void Core::decrement(char* byte){
    *byte = *byte - 1;
    setArithmaticFlags(*byte);
}

/**
 Adds the passed byte to the accumulator with the carry bit. If overflow occurs, the carry bit
 is also set. sets arithmatic flags.
 
 Changes: A, flags

 @param M The byte to be added to the accumulator.
 */
void Core::addWithCarry(char M){
    unsigned short A_u = promoteUnsigned(A);
    unsigned short M_u = promoteUnsigned(M);
    unsigned short tmp = A_u + M_u + getFlag(Flag::carry);
    setFlag(Flag::carry, tmp >= 0x100);  // set if value overflow
    A = static_cast<signed char>(tmp);
    setFlag(Flag::overflow, ((signed char)A_u + (signed char)M > 127) ||
                             ((signed char)A_u + (signed char)M < -128));
    setArithmaticFlags(A);
}

/**
 Subtracts the passed byte from the accumulator, taking into account the carry bit, i.e.
 A = A - M - (1 - C). If overflow occurs, the carry flag is cleared. Sets arithmatic flags.
 
 Changes: A, flags

 @param M the byte to subtract from A
 */
void Core::subWithCarry(char M){
    unsigned short A_u = promoteUnsigned(A);
    unsigned short M_u = promoteUnsigned(M);
    unsigned short tmp = A_u - M_u - (1 - getFlag(Flag::carry));
    setFlag(Flag::carry, tmp < 0x100); // clear if value overflow
    A = static_cast<signed char>(tmp);
    setFlag(Flag::overflow, ((signed char)A_u - (signed char)M > 127) ||
            ((signed char)A_u - (signed char)M < -128));
    setArithmaticFlags(A);
}

/**
 Perform an arithmatic shift left on a given real memory address and sets the
 arithmatic flags. The carry bit is set to the old bit 7.
 
 Changes: memory or A, flags

 @param byte pointer to the byte to perform the shift on.
 */
void Core::shiftLeft(char *byte){
    setFlag(Flag::carry, (*byte & NEGATIVE_FLAG) == NEGATIVE_FLAG); // carry <- old bit 7
    *byte = *byte << 1;
    setArithmaticFlags(*byte);
}

/**
 Perform an arithmatic shift left on a byte based on an address mode
 (including the "accumulator" address mode) and sets artithmatic flags.
 The carry bit is set to the old bit 7.
 
 Changes: memory or A, flags

 @param mode the address mode used to determine which byte to shift.
 */
void Core::shiftLeft(AddressMode mode){
    if(mode == acculumlator){
        shiftLeft(&A);
    } else {
        shiftLeft(m.getPointerAt(getAddress(mode)));
    }
}

/**
 Perform an arithmatic shift right on a given real memory address and sets the
 arithmatic flags. The carry bit is set to the old bit 0.
 
 Changes: memory or register, flags
 
 @param byte pointer to the byte to perform the shift on.
 */
void Core::shiftRight(char *byte){
    setFlag(Flag::carry, (*byte & CARRY_FLAG) == CARRY_FLAG); // old bit 0
    *byte = *byte >> 1;
    *byte = *byte & 0x7f;
    setArithmaticFlags(*byte);
}

/**
 Perform an arithmatic shift right on a byte based on an address mode
 (including the "accumulator" address mode) and sets artithmatic flags.
 The carry bit is set to the old bit 0.
 
 Changes: memory or A, flags
 
 @param mode the address mode used to determine which byte to shift.
 */
void Core::shiftRight(AddressMode mode){
    if(mode == acculumlator){
        shiftRight(&A);
    } else {
        shiftRight(m.getPointerAt(getAddress(mode)));
    }
}

/**
 Rotate each of the bits at the passed pointer to the left. i.e. shift all bits one
 place to the left, set bit 0 to the old value of the carry flag, and set the carry flag
 to the old value of bit 7.

 Changes: memory or register, flags
 
 @param byte pointer to the byte to be rotated.
 */
void Core::rotateLeft(char *byte){
    bool oldCarry = getFlag(Flag::carry);
    setFlag(Flag::carry, (*byte & NEGATIVE_FLAG) == NEGATIVE_FLAG); // carry <- old bit 7
    *byte = *byte << 1;
    *byte = *byte | oldCarry;
    setArithmaticFlags(*byte);
}

/**
 Rotate each of the bits at the address determined by the address mode left. i.e. shift all bits
 one place to the left, set bit 0 to the old value of the carry flag, and set the carry flag
 to the old value of bit 7.
 
 Changes: memory or A, flags
 
 @param mode the addressMode used to determine which byte to rotate
*/
void Core::rotateLeft(AddressMode mode){
    if(mode == acculumlator){
        rotateLeft(&A);
    } else {
        rotateLeft(m.getPointerAt(getAddress(mode)));
    }
}

/**
 Rotate each of the bits at the passed pointer to the right. i.e. shift all bits one
 place to the right, set bit 7 with the old value of the carry flag and set carry to
 the old value of bit 0
 
 Changes: memory or register, flags
 
 @param byte pointer to the byte to be rotated.
 */
void Core::rotateRight(char *byte){
    bool oldCarry = getFlag(Flag::carry);
    setFlag(Flag::carry, *byte & CARRY_FLAG); // carry <- old bit 0
    *byte = *byte >> 1;
    *byte = *byte & 0x7f;
    *byte = *byte | ((oldCarry << 7) & NEGATIVE_FLAG);
    setArithmaticFlags(*byte);
}

/**
 Rotate each of the bits at the address determined by the address mode right. i.e. shift all
 bits one place to the right, set bit 7 with the old value of the carry flag and set carry to
 the old value of bit 0
 
 Changes: memory or A, flags
 
 @param mode the addressMode used to determine which byte to rotate
*/
void Core::rotateRight(AddressMode mode){
    if(mode == acculumlator){
        rotateRight(&A);
    } else {
        rotateRight(m.getPointerAt(getAddress(mode)));
    }
}


/**
 Compare the contents of a given register to a given byte and set flags accordingly. C is high if
 reg <= byte, Z is high if reg == byte, and N is high if bit 7 of (reg - byte) is high,
 otherwise all are set false.
 
 Changes: flags

 @param byte the byte to compare with the register
 @param reg the register to be compared with the byte
 */
void Core::compare(char byte, char reg){
    setArithmaticFlags(reg - byte);
    signed short reg_u = promoteUnsigned(reg);
    signed short byte_u = promoteUnsigned(byte);
    setFlag(Flag::carry, reg_u>= byte_u);
}


/**
 Bitwise test M with A. i.e. the mask pattern in A is ANDed with M to set or clear the Z flag,
 but the result is discarded. Bits 7 and 6 of M are copied into N and V respectivly.

 @param M The byte to bit test against.
 */
void Core::bit(char M){
    setFlag(Flag::zero, (A & M) == 0);
    setFlag(Flag::overflow, (M & Flag::overflow) == Flag::overflow);
    setFlag(Flag::negative, (M & Flag::negative) == Flag::negative);
}

/**
 Branch to a specfic offset according to an address found using a given addressMode

 @param mode the addressMode used to look for the offset value.
 */
void Core::branch(AddressMode mode){
    signed char offset = getByte(mode, true);
    clock ++;
    jump(PC + offset);
}


/**
 Branch to a specfic offset according to an address found using a given addressMode if and only
 if the passed condition is true. If the condition is false, the PC is incremented according
 to the addressMode.

 @param mode the addressMode used to look for the offset value
 @param condition the predicate that determines wheter or not to jump.
 */
void Core::branchConditional(AddressMode mode, bool condition){
    if(condition){
        branch(mode);
    } else {
        getAddress(mode); //Move the PC to the proper place, discard result
    }
}

/**
 Pull an address off the stack and adjust the SP appropriatly.
 
 Changes: SP

 @return the virtual memory address stored at the top of the stack
 */
unsigned short Core::pullAddress(){
    char low = 0;
    char high = 0;
    pull(&low);
    pull(&high);
    return ((unsigned short)(high) << 8) + (unsigned char)low;
}

/**
 Reset the virtual CPU -- setting the PC based on the reset
 vector in memory 0xFFFC
 
 Changes: PC
 */
void Core::reset(){
    PC = getIndirectWithWrapping(RESET_VECTOR_LOCATION);
}

// Utils

/**
 Set a given flag to a given boolean value

 Changes: flags
 
 @param flag the flag to be set
 @param val the value to set the flag to
 */
void Core::setFlag(Flag flag, bool val){
    if(val){
        flags = flags | flag;
    } else {
        flags = flags & ('\xff' - flag);
    }
}

/**
 Set the arithmatic flags, zero and negative, based on a given byte
 
 Changes: flags

 @param byte the byte used to set the flags
 */
void Core::setArithmaticFlags(char byte){
    setFlag(Flag::zero, byte == 0);
    setFlag(Flag::negative, (byte & Flag::negative) == Flag::negative);
}


/**
 Return the address mode associated with a given opcode

 @param opcode the opcode to use
 @return the addressMode of the given opcode
 */
Core::AddressMode Core::getAddressModeFromOpcode(char opcode) const {
    auto mode = addressModeMap[(unsigned char)opcode % 0x20];
    if(mode != special){
        return mode;
    }
    unsigned char col = (unsigned char)opcode % 0x20;
    // 0x80, 0xa0, 0xc0, 0xe0
    if(col == 0 && (unsigned char)opcode > 0x60){
        return immediete;
    }
    // JSR
    if (opcode == '\x20'){
        return absolute;
    }
    // column 0x0C
    if(col == 0x0c){
        if(opcode == 0x6C){
            return indirect;
        } else {
            return absolute;
        }
    }
    // column 0x16
    if(col == 0x16){
        if(opcode == '\x96' || opcode == '\xb6'){
            return zeroPageY;
        } else {
            return zeroPageX;
        }
    }
    // column 0x17
    if(col == 0x17){
        if(opcode == '\x97' || opcode == '\xb7'){
            return zeroPageY;
        } else {
            return zeroPageX;
        }
    }
    // column 0x1E
    if(col == 0x1E){
        if(opcode == '\x9e' || opcode == '\xbe'){
            return absoluteY;
        } else {
            return absoluteX;
        }
    }
    // column 0x1F
    if(col == 0x1F){
        if(opcode == '\x9f' || opcode == '\xbf'){
            return absoluteY;
        } else {
            return absoluteX;
        }
    }
    
    return implicit;
}

/**
 Return the virtual address stored in virtual memory at a given location with
 all of the neccisary wrapping. i.e. if 10 20 are stored at memory location
 0x0000, the getIndirectWithWrapping(0x0) will return 0x2010.

 @param address the first virtual address location where the two bytes for
 the target address are stored
 @return the virtual memory address represented by the two bytes stored at the
 given address
 */
inline unsigned short Core::getIndirectWithWrapping(unsigned short address) const {
    return promoteUnsigned(m.getByte(address)) + (m.getByte(((address + 1 )& 0xff) + ((address >> 8) << 8)) << 8);
}

/**
 Print the state of the CPU, including A, X, Y, the SP, and the flags
 */
void Core::debugPrintCPU() const {
    printf("A:%2x\tX:%2x\tY:%2x\tSP:%2x\tf:%2x\n",
           Utils::printable(A),
           Utils::printable(X),
           Utils::printable(Y),
           Utils::printable(SP),
           Utils::printable(flags));
}

// Public Utility/testing methods

/**
 Load a short snippit of 6502 compiled bytecode (as a std::string of hex) into the virtual
 CPU at 0x8000 and set the PC there. Intened for testing purposes only

 @param hex_chars the compiled bytecode to be loaded in
 */
void Core::loadProgram(std::string hex_chars){
    loadIntoMemory(hex_chars, TESTING_PRG_OFFSET);
    setPC(TESTING_PRG_OFFSET);
    clock = 0;
}

/**
 Run a short snippit of compiled 6502 bytecode with a freshly reset virtual CPU. After running
 prints the ending clock time to cout. Intended for testinf purposes only. 

 @param hex_chars the compiled bytecode to be run.
 */
void Core::runProgram(std::string hex_chars){
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
