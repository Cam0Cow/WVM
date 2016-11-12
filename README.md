# WVM

## What is WVM?
WVM is an emulator for a nonexistent machine, aptly named the Weird Virtual
Machine the WVM has 64k emulated RAM, with two hardware stacks, perfect for
FORTH like languages.


## Overview
As well as being dual-stack, the WVM is also dual-size oriented; each
instruction has both a byte variant and a cell (16 bit) variant. The implemented
opcodes includes these:

BRK 0
NOP 1
PUSHB 2
PUSHC 3
ADDB 4
SUBB 5
MULB 6
DIVB 7
ADDC 8
SUBC 9
MULC 10
DIVC 11
LOGB 12
PUTB 13
LOGC 14
PUTC 15
DUPB 16
DROPB 17
SWAPB 18
ROTB 19
DUPC 20
DROPC 21
SWAPC 22
ROTC 23
IN 24
JZ 25
JMP 26
LDB 27
STRB 28
LDC 29
STRC 30
