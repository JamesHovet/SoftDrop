//
//  CoreTests.m
//  CoreTests
//
//  Created by James Hovet on 7/11/19.
//  Copyright © 2019 James Hovet. All rights reserved.
//

#import <XCTest/XCTest.h>

#define private public //EEEEEEEVIL Hack to test against private members

#include "../src/Core.hpp"

@interface CoreTests : XCTestCase

@end

@implementation CoreTests

Core cpu;

- (void) testLDA {
    /* LDA Immediete & Zero Flag */
    cpu.runProgram("a9 00 00");
    XCTAssert(cpu.A == 0);
    XCTAssert(cpu.getFlag(Core::Flag::zero));
    // XCTAssert(cpu.clock == 3);
    
    /* Negative Flag */
    cpu.runProgram("a9 f0 00");
    XCTAssert(cpu.getFlag(Core::Flag::negative));
    
    /* LDA zero page
     Address  Hexdump   Dissassembly
     -------------------------------
     $0600    a9 55     LDA #$55   2
     $0602    85 00     STA $00    3
     $0604    a9 00     LDA #$00   2
     $0606    a5 00     LDA $00    3
     */
    cpu.runProgram("a9 55 85 00 a9 00 a5 00 00");
    XCTAssert(cpu.A == '\x55');
    // XCTAssert(cpu.clock == 11);
    
    /* LDA zero page indexed
     Address  Hexdump   Dissassembly
     -------------------------------
     $0600    a9 55     LDA #$55   2
     $0602    85 01     STA $01    3
     $0604    a2 01     LDX #$01   2
     $0606    b5 00     LDA $00,X  4
     */
    cpu.runProgram("a9 55 85 01 a2 01 b5 00 00");
    XCTAssert(cpu.A == '\x55');
    // XCTAssert(cpu.clock == 12);
    
    /* LDA Absolute
     Address  Hexdump   Dissassembly
     -------------------------------
     $0600    a9 55     LDA #$55   2
     $0602    85 20     STA $20    3
     $0604    ad 20 00  LDA $0001  4
     */
    cpu.runProgram("a9 55 85 20 ad 20 00");
    XCTAssert(cpu.A == '\x55');
    // XCTAssert(cpu.clock == 10);
    
    /* LDA Absolute,X (with page boundary cross)
     Address  Hexdump   Dissassembly
     -------------------------------
     $0600    a9 55     LDA #$55         2
     $0602    8d 00 01  STA $0100        4
     $0605    a9 00     LDA #$00         2
     $0607    a2 01     LDX #$01         2
     $0609    bd ff 00  LDA $00ff,X      4 + 1
     */
    cpu.runProgram("a9 55 8d 00 01 a9 00 a2 01 bd ff 00 00");
    XCTAssert(cpu.A == '\x55');
    
    /* LDA Absolute,Y (essentially same as above)*/
    cpu.runProgram("a9 55 8d 00 01 a9 00 a0 01 b9 ff 00 00");
    XCTAssert(cpu.A == '\x55');
    
    /* LDA Indexed Indirect
     Address  Hexdump   Dissassembly
     -------------------------------
     $0600    a2 01     LDX #$01    2
     $0602    a9 05     LDA #$05    2
     $0604    85 01     STA $01     3
     $0606    a9 02     LDA #$02    2
     $0608    85 02     STA $02     3
     $060a    a9 55     LDA #$55    2
     $060c    8d 05 02  STA $0205   4
     $060f    a1 00     LDA ($00,X) 6
     */
    cpu.runProgram("a2 01 a9 05 85 01 a9 02 85 02 a9 55 8d 05 02 a1 00 00");
    XCTAssert(cpu.A == '\x55');
    // XCTAssert(cpu.clock == 25);
    
    /* LDA Indirect Indexed
     Address  Hexdump   Dissassembly
     -------------------------------
     $0600    a0 01     LDY #$01     2
     $0602    a9 03     LDA #$03     2
     $0604    85 01     STA $01      3
     $0606    a9 07     LDA #$07     2
     $0608    85 02     STA $02      3
     $060a    a2 55     LDX #$55     2
     $060c    8e 04 07  STX $0704    4
     $060f    b1 01     LDA ($01),Y  5
     */
    cpu.runProgram("a0 01 a9 03 85 01 a9 07 85 02 a2 55 8e 04 07 b1 01 00");
    XCTAssert(cpu.A == '\x55');
    // XCTAssert(cpu.clock == 24);
}

- (void) testLDX {
    /* LDX Immediete*/
    cpu.runProgram("a2 01 00");
    XCTAssert(cpu.X == 0x1);
    // XCTAssert(cpu.clock == 3);
    
    /* LDX Zero Page
     Address  Hexdump   Dissassembly
     -------------------------------
     $0600    a9 55     LDA #$55   2
     $0602    85 10     STA $10    3
     $0604    a6 10     LDX $10    3
     */
    cpu.runProgram("a9 55 85 10 a6 10 00");
    XCTAssert(cpu.X == '\x55');
    // XCTAssert(cpu.clock == 9);
    
    /* LDX Zero Page,Y
     Address  Hexdump   Dissassembly
     -------------------------------
     $0600    a9 55     LDA #$55   2
     $0602    85 10     STA $10    3
     $0604    a0 10     LDY #$10   2
     $0606    b6 00     LDX $00,Y  4
     */
    cpu.runProgram("a9 55 85 10 a0 10 b6 00 00");
    XCTAssert(cpu.X == '\x55');
    // XCTAssert(cpu.clock == 12);
    
    /* LDX Absolute
     Address  Hexdump   Dissassembly
     -------------------------------
     $0600    a9 55     LDA #$55   2
     $0602    8d 23 01  STA $0123  4
     $0605    ae 23 01  LDX $0123  4
     */
    cpu.runProgram("a9 55 8d 23 01 ae 23 01 00");
    XCTAssert(cpu.X == '\x55');
    // XCTAssert(cpu.clock == 11);
    
    /* LDX Absolute,Y (no page cross)
     Address  Hexdump   Dissassembly
     -------------------------------
     $0600    a9 55     LDA #$55     2
     $0602    8d 23 01  STA $0123    4
     $0605    a0 22     LDY #$22     2
     $0607    be 01 01  LDX $0101,Y  4
     */
    cpu.runProgram("a9 55 8d 23 01 a0 22 be 01 01 00");
    XCTAssert(cpu.X == '\x55');
    // XCTAssert(cpu.clock == 13);
}

- (void) testLDY {
    /* LDY Immediete*/
    cpu.runProgram(" a0 01 00");
    XCTAssert(cpu.Y == 0x1);
    // XCTAssert(cpu.clock == 3);
}

- (void) testPageBoundaryCrossClock {
    
    /*
     Tests every address mode, but not every instruction that has a
     possible boundry cross.
    */
    
    /* LDA Absolute,X with page boundary cross
     Address  Hexdump   Dissassembly
     -------------------------------
     $0600    a9 55     LDA #$55         2
     $0602    8d 00 01  STA $0100        4
     $0605    a9 00     LDA #$00         2
     $0607    a2 01     LDX #$01         2
     $0609    bd ff 00  LDA $00ff,X      4 + 1
     BRK
    */
    cpu.runProgram("a9 55 8d 00 01 a9 00 a2 01 bd ff 00 00");
    XCTAssert(cpu.clock == 22);
    
    /* LDA Absolute,Y (essentially same as above)*/
    cpu.runProgram("a9 55 8d 00 01 a9 00 a0 01 b9 ff 00 00");
    XCTAssert(cpu.clock == 22);
    
    /* LDA Indirect indexed with page boundary cross
     Address  Hexdump   Dissassembly
     -------------------------------
     $0600    a0 11     LDY #$11      2
     $0602    a9 f0     LDA #$f0      2
     $0604    85 01     STA $01       3
     $0606    a9 01     LDA #$01      2
     $0608    85 02     STA $02       3
     $060a    a2 55     LDX #$55      2
     $060c    8e 01 02  STX $0201     4
     $060f    b1 01     LDA ($01),Y   5 + 1
     BRK                              7
    */
    cpu.runProgram("a0 11 a9 f0 85 01 a9 01 85 02 a2 55 8e 01 02 b1 01 00");
    XCTAssert(cpu.clock == 31);
}

- (void) testSTA {
    /* STA Zero Page
     Address  Hexdump   Dissassembly
     -------------------------------
     $0600    a9 55     LDA #$55    2
     $0602    85 00     STA $00     3
     $0604    a9 00     LDA #$00    2
     $0606    a5 00     LDA $00     3
     */
    cpu.runProgram("a9 55 85 00 a9 00 a5 00 00");
    XCTAssert(cpu.M[0] == '\x55');
    // XCTAssert(cpu.clock == 11);
    
    /* STA Zero Page,X
     Address  Hexdump   Dissassembly
     -------------------------------
     $0600    a9 55     LDA #$55
     $0602    a2 f0     LDX #$f0
     $0604    95 00     STA $00,X
     */
    cpu.runProgram("a9 55 a2 f0 95 00 00");
    XCTAssert(cpu.M[0xf0] == '\x55');
    
    /* STA Absolute
     Address  Hexdump   Dissassembly
     -------------------------------
     $0600    a9 55     LDA #$55     2
     $0602    8d 02 01  STA $0102    4
     */
    cpu.runProgram("a9 55 8d 02 01 00");
    XCTAssert(cpu.M[0x0102] == '\x55');
    XCTAssert(cpu.clock = 7);
    
    
}

- (void) testSTX {
    /* STX all modes
     Address  Hexdump   Dissassembly
     -------------------------------
     $0600    a2 55     LDX #$55    2
     $0602    86 00     STX $00     3
     $0604    a0 01     LDY #$01    2
     $0606    96 00     STX $00,Y   4
     $0608    8e 02 00  STX $0002   4
     */
    cpu.runProgram("a2 55 86 00 a0 01 96 00 8e 02 00 00");
    XCTAssert(cpu.M[0] == '\x55');
    XCTAssert(cpu.M[1] == '\x55');
    XCTAssert(cpu.M[2] == '\x55');
    // XCTAssert(cpu.clock == 16);
    
}

- (void) testSTY {
    /* STY all modes
     Address  Hexdump   Dissassembly
     -------------------------------
     $0600    a0 55     LDY #$55
     $0602    84 00     STY $00
     $0604    a2 01     LDX #$01
     $0606    94 00     STY $00,X
     $0608    8c 02 00  STY $0002
     */
    cpu.runProgram("a0 55 84 00 a2 01 94 00 8c 02 00 00");
    XCTAssert(cpu.M[0] == '\x55');
    XCTAssert(cpu.M[1] == '\x55');
    XCTAssert(cpu.M[2] == '\x55');
    // XCTAssert(cpu.clock == 16);
    
}

- (void) testStack {
    /* PHA
     Address  Hexdump   Dissassembly
     -------------------------------
     $0600    a9 55     LDA #$55
     $0602    48        PHA
     */
    cpu.runProgram("a9 55 48 00");
    XCTAssert(cpu.M[0x1ff] == '\x55');
    XCTAssert(cpu.SP == STACK_TOP - 1);
    
    /* PLA
     Address  Hexdump   Dissassembly
     -------------------------------
     $0600    a9 55     LDA #$55
     $0602    48        PHA
     $0603    a9 44     LDA #$44
     $0605    48        PHA
     $0606    68        PLA
     $0607    68        PLA
     */
    cpu.runProgram("a9 55 48 a9 44 48 68 68 00");
    XCTAssert(cpu.A == '\x55');
    XCTAssert(cpu.SP == STACK_TOP);
    
    /* PHP & PLP
     Address  Hexdump   Dissassembly
     -------------------------------
     $0600    a9 ff     LDA #$ff
     $0602    08        PHP
     $0603    a9 00     LDA #$00
     $0605    28        PLP
     */
    cpu.runProgram("a9 ff 08 a9 00 28 00");
    XCTAssert(cpu.getFlag(Core::Flag::negative));
    XCTAssert(!cpu.getFlag(Core::Flag::zero));
    
    /* TXS & TSX
     Address  Hexdump   Dissassembly
     -------------------------------
     $0600    a2 f0     LDX #$f0
     $0602    9a        TXS
     $0603    a2 44     LDX #$44
     $0605    ba        TSX
     */
    cpu.runProgram("a2 f0 9a a2 44 ba 00");
    XCTAssert(cpu.SP == 240);
    XCTAssert(cpu.X == '\xf0');
}

- (void) testSubRoutine {
    /* JSR
     Address  Hexdump   Dissassembly
     -------------------------------
     $0600    20 05 06  JSR $0605
     $0603    a9 44     LDA #$44
     $0605    a9 55     LDA #$55
     $0607    00        BRK
    */
    cpu.runProgram(""); // reset registers
    cpu.loadIntoMemory("20 05 06 a9 44 a9 55 00", 0x0600);
    cpu.setPC(0x0600);
    while(cpu.step() == 0);
    XCTAssert(cpu.A == '\x55');
    XCTAssert(cpu.SP == 253);
    XCTAssert(cpu.M[0x1ff] == '\x06');
    XCTAssert(cpu.M[0x1fe] == '\x02');
    
    /* RTS
     Address  Hexdump   Dissassembly
     -------------------------------
     $0600    20 06 06  JSR $0606
     $0603    a9 55     LDA #$55
     $0605    00        BRK
     $0606    60        RTS
    */
    cpu.runProgram(""); // reset registers
    cpu.loadIntoMemory("20 06 06 a9 55 00 60 00", 0x0600);
    cpu.setPC(0x0600);
    while(cpu.step() == 0);
    XCTAssert(cpu.A == '\x55');
    XCTAssert(cpu.SP == 255);
}

- (void) testShiftAndRotate {
    /* ASL */
    cpu.runProgram("a9 01 0a 00");
    XCTAssert(cpu.A == '\x02');
    XCTAssert(!cpu.getFlag(Core::Flag::carry));
    XCTAssert(!cpu.getFlag(Core::Flag::zero));
    
    cpu.runProgram("a9 80 0a 00");
    XCTAssert(cpu.A == '\x00');
    XCTAssert(cpu.getFlag(Core::Flag::carry));
    XCTAssert(cpu.getFlag(Core::Flag::zero));
    
    /* LSR */
    cpu.runProgram("a9 01 4a 00");
    XCTAssert(cpu.A == '\x00');
    XCTAssert(cpu.getFlag(Core::Flag::carry));
    XCTAssert(cpu.getFlag(Core::Flag::zero));
    
    cpu.runProgram("a9 80 4a 00");
    XCTAssert(cpu.A == '\x40');
    XCTAssert(!cpu.getFlag(Core::Flag::carry));
    XCTAssert(!cpu.getFlag(Core::Flag::zero));
    
    /* ROL */
    cpu.runProgram("a9 01 2a 00");
    XCTAssert(cpu.A == '\x02');
    XCTAssert(!cpu.getFlag(Core::Flag::carry));
    XCTAssert(!cpu.getFlag(Core::Flag::zero));
    
    cpu.runProgram("a9 80 2a 00");
    XCTAssert(cpu.A == '\x00');
    XCTAssert(cpu.getFlag(Core::Flag::carry));
    XCTAssert(cpu.getFlag(Core::Flag::zero));
    
    cpu.runProgram("a9 01 38 2a 00"); // with carry set
    XCTAssert(cpu.A == '\x03');
    XCTAssert(!cpu.getFlag(Core::Flag::carry));
    XCTAssert(!cpu.getFlag(Core::Flag::zero));
    
    /* ROR */
    cpu.runProgram("a9 01 6a 00");
    XCTAssert(cpu.A == '\x00');
    XCTAssert(cpu.getFlag(Core::Flag::carry));
    XCTAssert(cpu.getFlag(Core::Flag::zero));
    
    cpu.runProgram("a9 80 6a 00");
    XCTAssert(cpu.A == '\x40');
    XCTAssert(!cpu.getFlag(Core::Flag::carry));
    XCTAssert(!cpu.getFlag(Core::Flag::zero));
    
    cpu.runProgram("a9 01 38 6a 00");
    XCTAssert(cpu.A == '\x80');
    XCTAssert(cpu.getFlag(Core::Flag::carry));
    XCTAssert(!cpu.getFlag(Core::Flag::zero));
    
}

- (void) testIncrementDecrement {
    cpu.runProgram("a2 00 a0 00 e8 c8 00");
    XCTAssert(cpu.X == 1);
    XCTAssert(cpu.Y == 1);
    // XCTAssert(cpu.clock == 9);
    
    cpu.runProgram("a2 01 a0 01 ca 88 00");
    XCTAssert(cpu.X == 0);
    XCTAssert(cpu.Y == 0);
    XCTAssert(cpu.getFlag(Core::Flag::zero));
    // XCTAssert(cpu.clock == 9);
    
    cpu.runProgram("a2 00 ca 00");
    XCTAssert(cpu.X == '\xff');
    XCTAssert(cpu.getFlag(Core::Flag::negative));
    // XCTAssert(cpu.clock == 5);
    
    /* INC Zero Page
     Address  Hexdump   Dissassembly
     -------------------------------
     $0600    a9 54     LDA #$54
     $0602    85 01     STA $01
     $0604    e6 01     INC $01
     $0606    a5 01     LDA $01
     */
    cpu.runProgram("a9 54 85 01 e6 01 a5 01 00");
    XCTAssert(cpu.A == '\x55');
    
    /* DEC Zero Page & flags
     Address  Hexdump   Dissassembly
     -------------------------------
     $0600    a9 01     LDA #$01
     $0602    85 10     STA $10
     $0604    c6 10     DEC $10
     $0606    a5 10     LDA $10
     */
    cpu.runProgram("a9 01 85 10 c6 10 a5 10 00");
    XCTAssert(cpu.A == 0);
    XCTAssert(cpu.getFlag(Core::Flag::zero));
}

- (void) testADC {
    /* ADC zero page no overflow or carry (0x20 + 0x20)
     Address  Hexdump   Dissassembly
     -------------------------------
     $0600    a9 20     LDA #$20
     $0602    85 01     STA $01
     $0604    65 01     ADC $01
     */
    cpu.runProgram("a9 20 85 01 65 01 00");
    XCTAssert(cpu.A == '\x40');
    XCTAssert(!cpu.getFlag(Core::Flag::zero));
    XCTAssert(!cpu.getFlag(Core::Flag::overflow));
    XCTAssert(!cpu.getFlag(Core::Flag::negative));
    XCTAssert(!cpu.getFlag(Core::Flag::carry));
    
    /* ADC with overflow (0x70 + 0x70)*/
    cpu.runProgram("a9 70 85 01 65 01 00");
    XCTAssert(cpu.A == '\xe0');
    XCTAssert(!cpu.getFlag(Core::Flag::zero));
    XCTAssert(cpu.getFlag(Core::Flag::overflow));
    XCTAssert(cpu.getFlag(Core::Flag::negative));
    XCTAssert(!cpu.getFlag(Core::Flag::carry));
    
    /* ADC with carry and zero (0x80 + 0x80)*/
    cpu.runProgram("a9 80 85 01 65 01 00");
    XCTAssert(cpu.A == '\x00');
    XCTAssert(cpu.getFlag(Core::Flag::zero));
    XCTAssert(cpu.getFlag(Core::Flag::overflow));
    XCTAssert(!cpu.getFlag(Core::Flag::negative));
    XCTAssert(cpu.getFlag(Core::Flag::carry));
    
    /* ADC with small negatives (-16 + -16 or 0xf0 - 0xf0) */
    cpu.runProgram("a9 f0 85 01 65 01 00");
    XCTAssert(cpu.A == '\xe0');
    XCTAssert(!cpu.getFlag(Core::Flag::zero));
    XCTAssert(!cpu.getFlag(Core::Flag::overflow));
    XCTAssert(cpu.getFlag(Core::Flag::negative));
    XCTAssert(cpu.getFlag(Core::Flag::carry));
}

- (void) testSBC {
    /* SBC Immediete small positives (0x12, 0x01)
     Address  Hexdump   Dissassembly
     -------------------------------
     $0600    a9 12     LDA #$12
     $0602    e9 01     SBC #$01
     */
    cpu.runProgram("a9 12 e9 01 00");
    XCTAssert(cpu.A = '\x10');
    XCTAssert(!cpu.getFlag(Core::Flag::zero));
    XCTAssert(!cpu.getFlag(Core::Flag::overflow));
    XCTAssert(!cpu.getFlag(Core::Flag::negative));
    XCTAssert(cpu.getFlag(Core::Flag::carry));
    
    /* SBC Immediete small positives with SEC
     Address  Hexdump   Dissassembly
     -------------------------------
     $0600    38        SEC
     $0601    a9 12     LDA #$12
     $0603    e9 01     SBC #$01
     */
    cpu.runProgram("38 a9 12 e9 01 00");
    XCTAssert(cpu.A = '\x11');
    XCTAssert(!cpu.getFlag(Core::Flag::zero));
    XCTAssert(!cpu.getFlag(Core::Flag::overflow));
    XCTAssert(!cpu.getFlag(Core::Flag::negative));
    XCTAssert(cpu.getFlag(Core::Flag::carry));
    
    /* SBC Immediete A - B where B > A */
    cpu.runProgram("a9 10 e9 20 00");
    XCTAssert(cpu.A = '\xef');
    XCTAssert(!cpu.getFlag(Core::Flag::zero));
    XCTAssert(!cpu.getFlag(Core::Flag::overflow));
    XCTAssert(cpu.getFlag(Core::Flag::negative));
    XCTAssert(!cpu.getFlag(Core::Flag::carry));
    
    /* SBC Immediete A - B where A < 0 && B < 0 */
    cpu.runProgram("a9 80 e9 a0 00");
    XCTAssert(cpu.A = '\xdf');
    XCTAssert(!cpu.getFlag(Core::Flag::zero));
    XCTAssert(!cpu.getFlag(Core::Flag::overflow));
    XCTAssert(cpu.getFlag(Core::Flag::negative));
    XCTAssert(!cpu.getFlag(Core::Flag::carry));
    
}

- (void) testJMP {
    /* JMP Absolute */
    cpu.loadProgram("4c 01 00 00");
    cpu.step();
    XCTAssert(cpu.PC == 1);
    // XCTAssert(cpu.clock == 3);
    
    /* JMP Indirect
     Address  Hexdump   Dissassembly
     -------------------------------
     $0600    a9 34     LDA #$34
     $0602    8d f0 01  STA $01f0
     $0605    a9 12     LDA #$12
     $0607    8d f1 01  STA $01f1
     $060a    6c f0 01  JMP ($01f0)
     */
    cpu.runProgram("a9 34 8d f0 01 a9 12 8d f1 01 6c f0 01 00");
    XCTAssert(cpu.PC = 1235);
    
}

- (void) testComparison {
    /* CMP Immediete & zero flag*/
    cpu.runProgram("a9 01 c9 01 00");
    XCTAssert(cpu.getFlag(Core::Flag::zero));
    // XCTAssert(cpu.clock == 5);
    
    /* CMP carry flag */
    cpu.runProgram("a9 55 c9 01 00");
    XCTAssert(cpu.getFlag(Core::Flag::carry));
    
    /* CMP negative flag */
    cpu.runProgram("a9 10 c9 11 00");
    XCTAssert(cpu.getFlag(Core::Flag::negative));
    
    /* CPX Immediete */
    cpu.runProgram("a2 01 e0 01 00");
    XCTAssert(cpu.getFlag(Core::Flag::zero));
    
    /* CPY Immediete */
    cpu.runProgram("a0 01 c0 01 00");
    XCTAssert(cpu.getFlag(Core::Flag::zero));
    
}

- (void) testBranch {
    /* BNE, passing test
     Address  Hexdump   Dissassembly
     -------------------------------
     $0600    a9 01     LDA #$01
     $0602    c9 02     CMP #$02
     $0604    d0 01     BNE $0607
     $0606    00        BRK
     $0607    a9 55     LDA #$55
     */
    cpu.runProgram("a9 01 c9 02 d0 01 00 a9 55 00");
    XCTAssert(cpu.A == '\x55');
    
    /* BNE, failing test */
    cpu.runProgram("a9 01 c9 01 d0 01 00 a9 55 00");
    XCTAssert(cpu.A == '\x01'); // something wrong with the PC
    
    /* BNE, jumping above
     Address  Hexdump   Dissassembly
     -------------------------------
     $0600    a2 00     LDX #$00
     $0602    e8        INX
     $0603    e0 55     CPX #$55
     $0605    d0 fb     BNE $0602
     $0607    00        BRK
     */
    cpu.runProgram("a2 00 e8 e0 55 d0 fb 00");
    XCTAssert(cpu.X == '\x55');
    
    //TODO: clock test
    
}

- (void) testAND {
    /* AND zero page & flags
     Address  Hexdump   Dissassembly
     -------------------------------
     $0600    a9 00     LDA #$00  2
     $0602    a9 15     LDA #$15  2
     $0604    85 00     STA $00   3
     $0606    a9 51     LDA #$51  2
     $0608    25 00     AND $00   3
     */
    cpu.runProgram("a9 00 a9 15 85 00 a9 51 25 00 00");
    XCTAssert(cpu.A = '\x11');
    XCTAssert(!cpu.getFlag(Core::Flag::zero));
    // XCTAssert(cpu.clock == 13);
}

- (void) testEOR {
    /* EOR Immediate & flags
     Address  Hexdump   Dissassembly
     -------------------------------
     $0600    a9 50     LDA #$50
     $0602    49 05     EOR #$05
     */
    cpu.runProgram("a9 50 49 05 00");
    XCTAssert(cpu.A == '\x55');
    XCTAssert(!cpu.getFlag(Core::Flag::zero));
    XCTAssert(!cpu.getFlag(Core::Flag::negative));
    
    /* EOR Zero Page & flags
     Address  Hexdump   Dissassembly
     -------------------------------
     $0600    a9 50     LDA #$50
     $0602    85 01     STA $01
     $0604    a9 50     LDA #$50
     $0606    45 01     EOR $01
     */
    cpu.runProgram("a9 50 85 01 a9 50 45 01 00");
    XCTAssert(cpu.A == 0);
    XCTAssert(cpu.getFlag(Core::Flag::zero));
    XCTAssert(!cpu.getFlag(Core::Flag::negative));
    
}

- (void) testORA {
    /* ORA Zero Page & flags
     Address  Hexdump   Dissassembly
     -------------------------------
     $0600    a9 50     LDA #$50
     $0602    85 01     STA $01
     $0604    a9 05     LDA #$05
     $0606    05 01     ORA $01
     */
    cpu.runProgram("a9 50 85 01 a9 05 05 01 ");
    XCTAssert(cpu.A == '\x55');
    XCTAssert(!cpu.getFlag(Core::Flag::zero));
    XCTAssert(!cpu.getFlag(Core::Flag::negative));
    
}












//- (void)setUp {
//    // Put setup code here. This method is called before the invocation of each test method in the class.
//}
//
//- (void)tearDown {
//    // Put teardown code here. This method is called after the invocation of each test method in the class.
//}

//- (void)testPerformanceExample {
//    // This is an example of a performance test case.
//    [self measureBlock:^{
//        // Put the code you want to measure the time of here.
//        /* Fill 0x00 to 0xff - 1 with their respective indecies
//         Address  Hexdump   Dissassembly
//         -------------------------------
//         $0600    a2 00     LDX #$00
//         $0602    a0 00     LDY #$00
//         $0604    96 00     STX $00,Y
//         $0606    e8        INX
//         $0607    c8        INY
//         $0608    c0 ff     CPY #$ff
//         $060a    d0 f8     BNE $0604
//         $060c    00        BRK
//        */
//        cpu.runProgram("a2 00 a0 00 96 00 e8 c8 c0 ff d0 f8 00");
//    }];
//}

@end
