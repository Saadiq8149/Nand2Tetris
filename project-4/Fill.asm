// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/4/Fill.asm

// Runs an infinite loop that listens to the keyboard input. 
// When a key is pressed (any key), the program blackens the screen,
// i.e. writes "black" in every pixel. When no key is pressed, 
// the screen should be cleared.

// LOOP:
//    if keyboard == 0:
//        screen.fill(white)
//    else:
//        screen.fill(black) 

(CHECK_KEYBOARD)
	// last pixel of screen
	@24575 
	D=A
	@current
	M=D

	// keyboard
	@24576
	D=M
	@fill_value
	M=-1

	@DRAW
	D;JNE

	@fill_value
	M=0


(DRAW)
	@fill_value
	D=M
	@current
	A=M
	M=D
	
	@current
	D=M
	// first pixel of screen
	@16384
	D=D-A
	@CHECK_KEYBOARD
	D;JEQ

	@current
	M=M-1
	@DRAW
	0;JMP







