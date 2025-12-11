// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/4/Mult.asm

// Multiplies R0 and R1 and stores the result in R2.
// (R0, R1, R2 refer to RAM[0], RAM[1], and RAM[2], respectively.)
// The algorithm is based on repetitive addition.

// a = RAM[R0]
// b = RAM[R1]
// i = 0
// mul = 0
// 
// LOOP:
//      if i >= b STOP
//      mul = mul + a
//      i = i + 1
// STOP:
//      RAM[R2] = mul

(SETUP)
    //a
    @R0
    D=M
    @a
    M=D

    //b
    @R1
    D=M
    @b
    M=D

    //i
    @i
    M=0

    //mul
    @mul
    M=0

(LOOP)
    // if i >= b GOTO STOP
    @i
    D=M
    @b
    D=D-M
    @STOP
    D;JGE

    // mul = mul + a
    @a
    D=M
    @mul
    M=D+M

    // i = i + 1
    @i
    M=M+1
    
    @LOOP
    0;JMP

(STOP)
    // R3 = a
    @mul
    D=M
    @R2
    M=D

(END)
    @END
    0;JMP


