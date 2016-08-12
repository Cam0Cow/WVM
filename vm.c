#include <stdlib.h>
#include <stdio.h>
#include "vm.h"

byte memory[1<<16];
uint16_t pc;
int zflag; // zero flag
int cflag; // carry flag
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
  return (cell_t) (upper << 8) & (lower);
}

byte fetch(void) {
  //printf("fetched byte %d at pc=%d\n", memory[pc], pc);
  return memory[pc++];
}

void execute(byte op) {
  switch (op) {
    case BRK: {
      pc = 0xFFFF; // terminate program
      break;
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
      cell_t c = (upper << 8) & (lower);
      dpushc(c);
      break;
    }
    case ADDB: {
      byte a = dpopb();
      byte b = dpopb();
      int result = b + a;
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
      byte result = b - a;
      cell_t check = (cell_t) b - (cell_t) a; //
    }
    case LOGB: {
      byte b = dpopb();
      printf("%d", b);
      break;
    }
  }
}

int loadFile(char *filename) {
  pc = 8092;
  FILE *file = fopen(filename, "r");
  fread((void *) text, 57444, 1, file);
  if (ferror(file)) {
    fclose(file);
    return -1;
  }
  fclose(file);
  return 0;
}
