#ifndef vm_h
#define vm_h
#include <stdint.h>

#define cell_t int16_t
#define byte int8_t

/*
* This is the list of opcodes supported on this VM
*/
#define BRK 0 // terminate VM
#define NOP 1 // no operation
#define PUSHB 2 // push byte to data stack
#define PUSHC 3 // push cell to data stack
#define ADDB 4 // add two bytes from dstack, place result on dstack
#define SUBB 5 // sub two bytes from dstack, place result on dstack
#define MULB 6 // mul two bytes from dstack, place result on dstack
#define DIVB 7 // div two bytes from dstack, place result on dstack
#define ADDC 8 // add two cells from dstack, place result on dstack
#define SUBC 9 // sub two cells from dstack, place result on dstack
#define MULC 10 // mul two cells from dstack, place result on dstack
#define DIVC 11 // div two cells from dstack, place result on dstack
#define LOGB 12 // print byte as a signed 8-bit integer
#define PUTB 13 // print byte as an ASCII character
#define LOGC 14 // print cell as a signed 16-bit integer
#define PUTC 15 // print cell as two ASCII characters


void dpushb(byte); // push byte onto data stack
byte dpopb(void); // pop byte from data stack
void dpushc(cell_t); // push cell onto data stack
cell_t dpopc(void); // pop cell off data stack
void rpush(cell_t); // push cell onto return stack
cell_t rpop(cell_t); // pop cell off return stack
byte fetch(void); // fetch next instruction
void execute(byte); // execute instruction
int loadFile(char *); // load file to memory

#endif
