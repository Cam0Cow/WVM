#ifndef vm_h
#define vm_h
#include <stdint.h>

#define cell_t int16_t
#define byte int8_t

/*
* This is the list of opcodes supported on this VM
* Note that that ADDB, SUBB, ADDC, and SUBC all use carry/borrow
* MULB returns cell_t, while MULC returns 2 cell_t, least significant on top
* DIVB divide cell_t by byte, return quotient and remainder (8 bit)
*/
#define BRK 0 // terminate VM
#define NOP 1 // no operation
#define PUSHB 2 // push byte to data stack
#define PUSHC 3 // push cell to data stack
#define ADDB 4 // see above for detail
#define SUBB 5 // see above for detail
#define MULB 6 // see above for detail
#define DIVB 7 // see above for detail
#define ADDC 8 // see above for detail
#define SUBC 9 // see above for detail
#define MULC 10 // see above for detail
#define DIVC 11 // see above for detail
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
int execute(byte); // execute instruction
int loadFile(char *); // load file to memory

#endif
