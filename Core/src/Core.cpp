//
//  core.cpp
//  jNES
//
//  Created by James Hovet on 7/11/19.
//  Copyright © 2019 James Hovet. All rights reserved.
//

#define CUSTOM_DEBUG

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
const int debugOpBytes[] = {
    1,2,0,0,0,2,2,0,1,2,1,0,0,3,3,0,2,2,0,0,0,2,2,0,1,3,0,0,0,3,3,0,3,2,0,0,2,2,2,0,1,2,1,0,3,3,3,0,2,2,0,0,0,2,2,0,1,3,0,0,0,3,3,0,1,2,0,0,0,2,2,0,1,2,1,0,3,3,3,0,2,2,0,0,0,2,2,0,1,3,0,0,0,3,3,0,1,2,0,0,0,2,2,0,1,2,1,0,3,3,3,0,2,2,0,0,0,2,2,0,1,3,0,0,0,3,3,0,0,2,0,0,2,2,2,0,1,0,1,0,3,3,3,0,2,2,0,0,2,2,2,0,0,3,1,0,0,3,0,0,2,2,2,0,2,2,2,0,1,2,1,0,3,3,3,0,2,2,0,0,2,2,2,0,1,3,1,0,3,3,3,0,2,2,0,0,2,2,2,0,1,2,1,0,3,3,3,0,2,2,0,0,0,2,2,0,1,3,0,0,0,3,3,0,2,2,0,0,2,2,2,0,1,2,1,0,3,3,3,0,2,2,0,0,0,2,2,0,1,3,0,0,0,3,3,0
};

inline static int printable(char byte){
    return (((unsigned short)byte)&0xff);
}

inline static unsigned short promoteUnsigned(char byte){
    return (((unsigned short)byte)&0xff);
}

inline static signed short promoteSigned(char byte){
    return (((signed short)byte));
}

//Core::Core(){
//    //TODO: Remove
//}

Core::Core(Mapper* mapper){
    m = mapper;
}

Core::~Core(){
    
}

void Core::loadIntoMemory(char* bytes, unsigned short address, unsigned short length){
    memcpy(m->getPointerAt(address), bytes, length);
}

void Core::loadIntoMemory(std::string hex_chars, unsigned short address){
    std::istringstream hex_chars_stream(hex_chars);
    
    unsigned int c;
    while (hex_chars_stream >> std::hex >> c)
    {
        m->setByte(address++, (char)c);
    }
}

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

int Core::step(){
    
    char op = m->getByte(PC++);
    unsigned char op_u = (unsigned char)op;
    AddressMode mode = getAddressMode(op);
    
    //debug
#ifdef CUSTOM_DEBUG
    
    printf("$%04x:%02x", PC - 1, printable(op));
    printf("\t");
    printf("A:%2x\tX:%2x\tY:%2x\tSP:%2x\tf:%2x\t",
           printable(A),
           printable(X),
           printable(Y),
           printable(SP),
           printable(flags));
    
    std::cout << debugOpNames[op_u];
    
    for(int i = 0; i < debugOpBytes[op_u] - 1; i++){
        std::cout << " " << std::hex << printable(m->getByte(PC + i));
    }
    
    printf("\tCYC:%lu", clock);
    
    std::cout << std::endl;
#endif
    
    clock += cycleCounts[op_u];
    
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
//            printf("possible error %02x on PC:%04x\n", X, PC);
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

unsigned short Core::getAddress(AddressMode mode){
    unsigned short address = PC + 1;
    switch (mode) {
        case AddressMode::immediete:
            return PC++;
        case AddressMode::offset:
            return PC++;
        case AddressMode::zeroPage:
            return promoteUnsigned(m->getByte(PC++));
        case AddressMode::zeroPageX:
            return (promoteUnsigned(m->getByte(PC++)) + promoteUnsigned(X)) % 0x100;
        case AddressMode::zeroPageY:
            return (promoteUnsigned(m->getByte(PC++)) + promoteUnsigned(Y)) % 0x100;
        case AddressMode::absolute:
            address = promoteUnsigned(m->getByte(PC)) + (promoteUnsigned(m->getByte(PC + 1)) << 8);
            PC += 2;
            return address; // return early to avoid triggering the page boundary code below
        case AddressMode::absoluteX:
            address = promoteUnsigned(m->getByte(PC)) + (promoteUnsigned(m->getByte(PC + 1)) << 8);
            address += promoteUnsigned(X);
            PC += 2;
            break;
        case AddressMode::absoluteY:
            address = promoteUnsigned(m->getByte(PC)) + (promoteUnsigned(m->getByte(PC + 1)) << 8);
            address += promoteUnsigned(Y);
            PC += 2;
            break;
        case AddressMode::indirect:
            address = promoteUnsigned(m->getByte(PC)) + (promoteUnsigned(m->getByte(PC + 1)) << 8);
            address = getIndirectWithWrapping(address);
            PC +=2;
            break;
        case AddressMode::indexedIndirect:
            address = getIndirectWithWrapping(promoteUnsigned(m->getByte(PC)) + X);
            PC++;
            break;
        case AddressMode::indirectIndexed:
            address = getIndirectWithWrapping(promoteUnsigned(m->getByte(PC))) + promoteUnsigned(Y);
            PC++;
            break;
        default:
            break;
    }
    
    return address;
}

unsigned short Core::getAddress(AddressMode mode, bool shouldCheckPageOverflow){
    if(shouldCheckPageOverflow &&
       (mode == absoluteX || mode == absoluteY || mode == indirectIndexed)
       ){
        unsigned short a1 = 0;
        unsigned short a2 = 0;
        if(mode == absoluteX){
            a1 = promoteUnsigned(m->getByte(PC)) + (promoteUnsigned(m->getByte(PC + 1)) << 8);
            a2 = a1 + promoteUnsigned(X);
            PC += 2;
        } else if (mode == absoluteY){
            a1 = promoteUnsigned(m->getByte(PC)) + (promoteUnsigned(m->getByte(PC + 1)) << 8);
            a2 = a1 + promoteUnsigned(Y);
            PC += 2;
        } else if (mode == indirectIndexed){
            a1 = getIndirectWithWrapping(promoteUnsigned(m->getByte(PC)));
            a2 = a1 + promoteUnsigned(Y);
            PC++;
        }
        handleCheckPageOverflow(a1, a2);
        return a2;
    } else {
        return getAddress(mode);
    }
}


void Core::loadRegister(char byte, char* reg){
    *reg = byte;
    setArithmaticFlags(byte);
}

void Core::increment(char* byte){
    *byte = *byte + 1;
    setArithmaticFlags(*byte);
}

void Core::decrement(char* byte){
    *byte = *byte - 1;
    setArithmaticFlags(*byte);
}

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

void Core::shiftLeft(char *byte){
    setFlag(Flag::carry, (*byte & NEGATIVE_FLAG) == NEGATIVE_FLAG); // carry <- old bit 7
    *byte = *byte << 1;
    setArithmaticFlags(*byte);
}

void Core::shiftLeft(AddressMode mode){
    if(mode == acculumlator){
        shiftLeft(&A);
    } else {
        shiftLeft(m->getPointerAt(getAddress(mode)));
    }
}

void Core::shiftRight(char *byte){
    setFlag(Flag::carry, (*byte & CARRY_FLAG) == CARRY_FLAG); // old bit 0
    *byte = *byte >> 1;
    *byte = *byte & 0x7f;
    setArithmaticFlags(*byte);
}

void Core::shiftRight(AddressMode mode){
    if(mode == acculumlator){
        shiftRight(&A);
    } else {
        shiftRight(m->getPointerAt(getAddress(mode)));
    }
}

void Core::rotateLeft(char *byte){
    bool oldCarry = getFlag(Flag::carry);
    setFlag(Flag::carry, (*byte & NEGATIVE_FLAG) == NEGATIVE_FLAG); // carry <- old bit 7
    *byte = *byte << 1;
    *byte = *byte | oldCarry;
    setArithmaticFlags(*byte);
}

void Core::rotateLeft(AddressMode mode){
    if(mode == acculumlator){
        rotateLeft(&A);
    } else {
        rotateLeft(m->getPointerAt(getAddress(mode)));
    }
}

void Core::rotateRight(char *byte){
    bool oldCarry = getFlag(Flag::carry);
    setFlag(Flag::carry, *byte & CARRY_FLAG); // carry <- old bit 0
    *byte = *byte >> 1;
    *byte = *byte & 0x7f;
    *byte = *byte | ((oldCarry << 7) & NEGATIVE_FLAG);
    setArithmaticFlags(*byte);
}

void Core::rotateRight(AddressMode mode){
    if(mode == acculumlator){
        rotateRight(&A);
    } else {
        rotateRight(m->getPointerAt(getAddress(mode)));
    }
}


void Core::compare(char byte, char reg){
    setArithmaticFlags(reg - byte);
    signed short reg_u = promoteUnsigned(reg);
    signed short byte_u = promoteUnsigned(byte);
    setFlag(Flag::carry, reg_u>= byte_u);
}

void Core::bit(char M){
    setFlag(Flag::zero, (A & M) == 0);
    setFlag(Flag::overflow, (M & Flag::overflow) == Flag::overflow);
    setFlag(Flag::negative, (M & Flag::negative) == Flag::negative);
}

void Core::branch(AddressMode mode){
    signed char offset = getByte(mode, true);
    clock ++;
    jump(PC + offset);
}

void Core::branchConditional(AddressMode mode, bool condition){
    if(condition){
        branch(mode);
    } else {
        getAddress(mode); //Move the PC to the proper place, discard result
    }
}

unsigned short Core::pullAddress(){
    char low = 0;
    char high = 0;
    pull(&low);
    pull(&high);
    return ((unsigned short)(high) << 8) + (unsigned char)low;
}

// Utils

void Core::setFlag(Flag flag, bool val){
    if(val){
        flags = flags | flag;
    } else {
        flags = flags & ('\xff' - flag);
    }
}

void Core::setArithmaticFlags(char byte){
    setFlag(Flag::zero, byte == 0);
    setFlag(Flag::negative, (byte & Flag::negative) == Flag::negative);
}

Core::AddressMode Core::getAddressMode(char opcode){
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

inline unsigned short Core::getIndirectWithWrapping(unsigned short address){
    return promoteUnsigned(m->getByte(address)) + (m->getByte(((address + 1 )& 0xff) + ((address >> 8) << 8)) << 8);
}

void Core::debugPrintCPU(){
    printf("A:%2x\tX:%2x\tY:%2x\tSP:%2x\tf:%2x\n",
           printable(A),
           printable(X),
           printable(Y),
           printable(SP),
           printable(flags));
}
