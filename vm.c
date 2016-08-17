#include <stdlib.h>
#include <stdio.h>
#include "vm.h"

byte memory[1<<16];
uint16_t pc;
int zflag; // zero flag
int cflag; // carry flag
int bflag = 1; // borrow flag

/*
* rstack: Return STACK => 1024 bytes long (512 cells/addresses)  0x0100-0x04ff
* dstack: Data   STACK => 1024 bytes long (512 cells/addresses)  0x0500-0x08ff
* data: multi purpose data storage, 5888 bytes long (2944 cells) 0x0900-0x2000
* note: both rstack and dstack count backwards from higher memory addressses,
* as stacks often do. The text pointer is unrelated to all, just marking the
* beginning of the text segment, where the code resides
*/
cell_t *rstack = (cell_t *) (memory + 1279);
byte *dstack = (memory + 2303);
byte *data  = (memory + 2304);
const byte *text = (memory + 8092);


static int dstackSize(void) {
  return dstack - memory + 1;
}

static int rstackSize(void) {
  return (byte *) rstack - memory + 1;
}

void dpushb(byte b) {
  if (!dstackSize()) {
    printf("stack overflow!\n"); // wrap around or interrupt later
    exit(0);
  }
  *dstack = b;
  --dstack;
}

byte dpopb(void) {
  ++dstack;
  return *dstack;
}

void dpushc(cell_t c) {
  *dstack = c >> 8; // upper part of cell_t
  --dstack;
  *dstack = c & 0xFF; // lower part of cell_t;
  --dstack;
}

cell_t dpopc(void) {
  ++dstack;
  byte lower = *dstack; // lower part of cell_t
  ++dstack;
  byte upper = *dstack; // upper part of cell_t

  return (cell_t) (upper << 8) | (uint8_t)(lower);
  // took me 3 hours to find out that the | should be that and
  // not a &. God dammit
}

byte fetch(void) {
  //printf("fetched byte %d at pc=%d\n", memory[pc], pc);
  return memory[pc++];
}

int execute(byte op) {
  switch (op) {
    case BRK: {
      pc = 0xFFFF; // terminate program
      return 0;
    }
    case NOP: {
      break; // nothing to see here
    }
    case PUSHB: {
      byte b = fetch();
      dpushb(b);
      break;
    }
    case PUSHC: {
      byte lower = fetch();
      byte upper = fetch();
      cell_t c = (upper << 8) | (lower);
      dpushc(c);
      break;
    }
    case ADDB: {
      byte a = dpopb();
      byte b = dpopb();
      int result = b + a + cflag;
      if (cflag) cflag = 0;
      if (result > 127) {
        cflag = 1;
        result -= 128;
      }
      zflag = (result == 0);
      dpushb((byte) result);
      break;
    }
    case SUBB: {
      byte a = dpopb();
      byte b = dpopb();
      int result = b + ~a + bflag;
      bflag = (result < 0);
      zflag = (result == 0);
      dpushb((byte) result);
      break;
    }
    case MULB: {
      // 8 bit x 8 bit => 16 bit
      byte a = dpopb();
      byte b = dpopb();
      int result = b * a;
      zflag = (result == 0);
      dpushc((cell_t) result);
      break;
    }
    case DIVB: {
      // 16 bit -:- 8 bit => 8bit & 8bit remainder
      byte a = dpopb();
      cell_t b = dpopc();
      int result = b / (cell_t) a;
      int r = b - (result * a); // remainder
      zflag = (result == 0);
      dpushb((byte) r);
      dpushb((byte) result);
      break;
    }
    case ADDC: {
      cell_t a = dpopc();
      cell_t b = dpopc();
      int result = b + a + cflag;
      printf("%d = %d + %d\n", result, b, a);
      if (cflag) cflag = 0;
      if (result > 65535) {
        cflag = 1;
        result -= 65535;
      }
      zflag = (result == 0);
      dpushc((cell_t) result);
      break;
    }
    case SUBC: {
      cell_t a = dpopc();
      cell_t b = dpopc();
      int result = b + ~a + bflag;
      bflag = (result < 0);
      zflag = (result == 0);
      dpushc((cell_t) result);
      break;
    }
    case MULC: {
      // 16 bit x 16 bit = high 16bit & lower 16bit
      cell_t a = dpopc();
      cell_t b = dpopc();
      int result = b * a;
      zflag = (result == 0);
      dpushc(result >> 16);
      dpushc(result & 0xFFFF);
      break;
    }
    case LOGB: {
      byte b = dpopb();
      printf("%d", b);
      break;
    }
    case LOGC: {
      cell_t c = dpopc();
      printf("%d", c);
      break;
    }
  }
  return 1;
}

int loadFile(char *filename) {
  pc = text - memory;
  FILE *file = fopen(filename, "r");
  fread((void *) text, 57444, 1, file);
  if (ferror(file)) {
    fclose(file);
    return -1;
  }
  fclose(file);
  return 0;
}
