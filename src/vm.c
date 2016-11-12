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
const byte *text = (memory + 8192);


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

byte dpeekb(void) {
  return *(dstack+1);
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

cell_t dpeekc(void) {
  byte lower = *(dstack + 1);
  byte upper = *(dstack + 2);
  return (cell_t) (upper << 8) | (uint8_t)(lower);
}

byte fetch(void) {
  //printf("fetched byte %d at pc=%d\n", memory[pc], pc);
  return memory[pc++];
}

int execute(byte op) {
  switch (op) {
    case BRK: {
      LOG("Terminating VM at pc %d\n", pc);
      pc = 0xFFFF; // terminate program
      return 0;
    }
    case NOP: {
      LOG("NOP(e)\n");
      break; // nothing to see here
    }
    case PUSHB: {
      byte b = fetch();
      LOG("Pushed byte %d at sp %ld\n", b, dstack - memory);
      dpushb(b);
      break;
    }
    case PUSHC: {
      byte lower = fetch();
      byte upper = fetch();
      cell_t c = (upper << 8) | (uint8_t)(lower);
      LOG("Pushed cell %d at sp %ld\n", c, dstack-memory);
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
      LOG("%d = %d + %d, cflag=%d, zflag=%d", result, b, a, cflag, zflag);
      LOG(" (bytes)\n");
      dpushb((byte) result);
      break;
    }
    case SUBB: {
      byte a = dpopb();
      byte b = dpopb();
      int result = b + ~a + bflag;
      bflag = !(result < 0);
      zflag = (result == 0);
      LOG("%d = %d - %d, bflag=%d, zflag=%d", result, b, a, bflag, zflag);
      LOG(" (bytes)\n");
      dpushb((byte) result);
      break;
    }
    case MULB: {
      // 8 bit x 8 bit => 16 bit
      byte a = dpopb();
      byte b = dpopb();
      int result = b * a;
      zflag = (result == 0);
      LOG("%d = %d * %d, zflag=%d", result, b, a, zflag);
      LOG(" (bytes)\n");
      dpushc((cell_t) result);
      break;
    }
    case DIVB: {
      // 16 bit -:- 8 bit => 8bit & 8bit remainder
      // result on top, remainder below
      byte a = dpopb();
      cell_t b = dpopc();
      int result = b / (cell_t) a;
      int r = b - (result * a); // remainder
      zflag = (result == 0);
      LOG("%d = %d / %d, remainder=%d, zflag=%d", result, b, a, r, zflag);
      LOG(" (bytes)\n");
      dpushb((byte) r);
      dpushb((byte) result);
      break;
    }
    case ADDC: {
      cell_t a = dpopc();
      cell_t b = dpopc();
      int result = b + a + cflag;
      if (cflag) cflag = 0;
      if (result > 65535) {
        cflag = 1;
        result -= 65535;
      }
      zflag = (result == 0);
      LOG("%d = %d + %d, cflag=%d, zflag=%d", result, b, a, cflag, zflag);
      LOG(" (cells)\n");
      dpushc((cell_t) result);
      break;
    }
    case SUBC: {
      cell_t a = dpopc();
      cell_t b = dpopc();
      int result = b + ~a + bflag;
      bflag = !(result < 0);
      zflag = (result == 0);
      LOG("%d = %d - %d, bflag=%d, zflag=%d", result, b, a, bflag, zflag);
      LOG(" (cells)\n");
      dpushc((cell_t) result);
      break;
    }
    case MULC: {
      // 16 bit x 16 bit = high 16bit & lower 16bit
      cell_t a = dpopc();
      cell_t b = dpopc();
      int result = b * a;
      zflag = (result == 0);
      LOG("%d = %d * %d, zflag=%d", result, b, a, zflag);
      LOG(" (cells)\n");
      dpushc(result >> 16);
      dpushc(result & 0xFFFF);
      break;
    }
    case DIVC: {
      // 32 bit -:- 16 bit = 16 bit & 16 bit remainder
      // result on top, remainder below
      cell_t a = dpopc();
      cell_t bl = dpopc(); // lower 16 bit
      cell_t bh = dpopc(); // higher 16 bit
      int b = ((bh << 16) | (uint16_t) bl);
      int result = b / a;
      int r = b - (result * a);
      zflag = (result == 0);
      LOG("%d = %d / %d, remainder=%d, zflag=%d", result, b, a, r, zflag);
      LOG(" (cells)\n");
      dpushc((cell_t) r);
      dpushc((cell_t) result);
      break;
    }
    case LOGB: {
      byte b = dpopb();
      printf("%d", b);
      LOG("printed byte as integer\n");
      break;
    }
    case PUTB: {
      byte b = dpopb();
      printf("%c", b);
      LOG("printed byte as character\n");
      break;
    }
    case LOGC: {
      cell_t c = dpopc();
      printf("%d", c);
      LOG("printed cell as integer\n");
      break;
    }
    case PUTC: {
      // big endian ordering here, for easy string manipulation
      byte a = dpopb();
      byte b = dpopb();
      printf("%c%c", a, b);
      LOG("printed cell as characters\n");
      break;
    }
    case DUPB: {
      byte a = dpeekb();
      dpushb(a);
      LOG("duplicated (byte) %d\n", a);
      break;
    }
    case DROPB :{
      (void) dpopb();
      LOG("dropped top byte\n");
      break;
    }
    case SWAPB: {
      byte a = dpopb();
      byte b = dpopb();
      dpushb(a);
      dpushb(b);
      LOG("swapped (bytes) %d and %d\n", a, b);
      break;
    }
    case ROTB: {
      byte a = dpopb();
      byte b = dpopb();
      byte c = dpopb();
      dpushb(b);
      dpushb(a);
      dpushb(c);
      LOG("rotated (bytes) %d, %d, %d\n", a, b, c);
      break;
    }
    case DUPC: {
      cell_t c = dpeekc();
      dpushc(c);
      LOG("duplicated (cell) %d\n", c);
      break;
    }
    case DROPC: {
      (void) dpopc();
      LOG("dropped top cell\n");
      break;
    }
    case SWAPC: {
      cell_t a = dpopc();
      cell_t b = dpopc();
      dpushc(a);
      dpushc(b);
      LOG("swapped (cells) %d and %d\n", a, b);
      break;
    }
    case ROTC: {
      cell_t a = dpopc();
      cell_t b = dpopc();
      cell_t c = dpopc();
      dpushc(b);
      dpushc(a);
      dpushc(c);
      LOG("rotated (cells) %d, %d, %d\n", a, b, c);
      break;
    }
    case IN: {
      char c = getchar();
      LOG("got character \'%c\'\n", c);
      dpushb(c);
      break;
    }
    case JT: {
      byte r = fetch();
      if (dpopb() == -1) {
        pc += r;
        LOG("branched to %d\n", pc);
      } else {
        LOG("branch not taken at pc = %d\n", pc);
      }
      break;
    }
    case JMP: {
      byte lower = fetch();
      byte upper = fetch();
      uint16_t c = (upper << 8) | (uint8_t)(lower);
      pc = c; // do the actual jump
      LOG("jumped to %d\n", c);
      break;
    }
    case LDB: {
      uint16_t a = dpopc();
      byte b = memory[a];
      LOG("fetched byte %d from memory location %d\n", b, a);
      dpushb(b);
      break;
    }
    case STRB: {
      byte b = dpopb();
      uint16_t a = dpopc();
      LOG("stored byte %d at memory location %d\n", b, a);
      memory[a] = b;
      break;
    }
    case LDC: {
      uint16_t a = dpopc();
      cell_t b = memory[a] | (memory[a+1] << 8);
      LOG("fetched cell %d from memory location %d\n", b, a);
      dpushc(b);
      break;
    }
    case STRC: {
      cell_t c = dpopc();
      uint16_t a = dpopc();
      memory[a] = c & 0xff;
      memory[a+1] = c >> 8;
      LOG("sotred cell %d at memory location %d\n", c, a);
      break;
    }
    case NOTB: {
      uint8_t b = dpopb();
      uint8_t nb = ~b;
      LOG("%d =  NOT %d (bytes)\n", nb, b);
      dpushb(nb);
      break;
    }
    case ANDB: {
      uint8_t a = dpopb();
      uint8_t b = dpopb();
      uint8_t c = a & b;
      LOG("%d = %d AND %d (bytes)\n", c, b, a);
      dpushb(c);
      break;
    }
    case ORB: {
      uint8_t a = dpopb();
      uint8_t b = dpopb();
      uint8_t c = a | b;
      LOG("%d = %d OR %d (bytes)\n", c, b, a);
      dpushb(c);
      break;
    }
    case XORB: {
      uint8_t a = dpopb();
      uint8_t b = dpopb();
      uint8_t c = a ^ b;
      LOG("%d = %d ^ %d (bytes)\n", c, b, a);
      dpushb(c);
      break;
    }
    case NOTC: {
      uint16_t a = dpopc();
      uint16_t na = ~a;
      LOG("%d = ~%d (cells)\n", na, a);
      dpushc(na);
      break;
    }
    case ANDC: {
      uint16_t a = dpopc();
      uint16_t b = dpopc();
      uint16_t c = a & b;
      LOG("%d = %d & %d (cells)\n", c, b, a);
      dpushc(c);
      break;
    }
    case ORC: {
      uint16_t a = dpopc();
      uint16_t b = dpopc();
      uint16_t c = a | b;
      LOG("%d = %d | %d (cells)\n", c, b, a);
      dpushc(c);
      break;
    }
    case XORC: {
      uint16_t a = dpopc();
      uint16_t b = dpopc();
      uint16_t c = a ^ b;
      LOG("%d = %d ^ %d (cells)\n", c, b, a);
      dpushc(c);
      break;
    }
    case LTB: {
      byte a = dpopb();
      byte b = dpopb();
      byte r = (b < a) ? -1 : 0;
      LOG("%d < %d (bytes)? %s\n", b, a, r ? "true" : "false");
      dpushb(r);
      break;
    }
    case LEB: {
      byte a = dpopb();
      byte b = dpopb();
      byte r = (b <= a) ? -1 : 0;
      LOG("%d <= %d (bytes)? %s\n", b, a, r ? "true" : "false");
      dpushb(r);
      break;
    }
    case EQB: {
      byte a = dpopb();
      byte b = dpopb();
      byte r = (b == a) ? -1 : 0;
      LOG("%d == %d (bytes)? %s\n", b, a, r ? "true" : "false");
      dpushb(r);
      break;
    }
    case GTB: {
      byte a = dpopb();
      byte b = dpopb();
      byte r = (b > a) ? -1 : 0;
      LOG("%d > %d (bytes)? %s\n", b, a, r ? "true" : "false");
      dpushb(r);
      break;
    }
    case GEB: {
      byte a = dpopb();
      byte b = dpopb();
      byte r = (b >= a) ? -1 : 0;
      LOG("%d >= %d (bytes)? %s\n", b, a, r ? "true" : "false");
      dpushb(r);
      break;
    }
    case LTC: {
      cell_t a = dpopb();
      cell_t b = dpopb();
      cell_t r = (b < a) ? -1 : 0;
      LOG("%d < %d (cells)? %s\n", b, a, r ? "true" : "false");
      dpushc(r);
      break;
    }
    case LEC: {
      cell_t a = dpopb();
      cell_t b = dpopb();
      cell_t r = (b <= a) ? -1 : 0;
      LOG("%d <= %d (cells)? %s\n", b, a, r ? "true" : "false");
      dpushc(r);
      break;
    }
    case EQC: {
      cell_t a = dpopb();
      cell_t b = dpopb();
      cell_t r = (b == a) ? -1 : 0;
      LOG("%d == %d (cells)? %s\n", b, a, r ? "true" : "false");
      dpushc(r);
      break;
    }
    case GTC: {
      cell_t a = dpopb();
      cell_t b = dpopb();
      cell_t r = (b > a) ? -1 : 0;
      LOG("%d > %d (cells)? %s\n", b, a, r ? "true" : "false");
      dpushc(r);
      break;
    }
    case GEC: {
      cell_t a = dpopb();
      cell_t b = dpopb();
      cell_t r = (b >= a) ? -1 : 0;
      LOG("%d >= %d (cells)? %s\n", b, a, r ? "true" : "false");
      dpushc(r);
      break;
    }
    case SEB: {
      byte b = dpopb();
      cell_t bx = (cell_t) b;
      LOG("sign extended %d as signed\n", bx);
      dpushc(bx);
      break;
    }
    case UEB: {
      uint8_t a = dpopb();
      uint16_t exa = (uint16_t) a;
      LOG("signed extended %d as unsigned\n", exa);
      dpushc(exa);
      break;
    }
  }
  return 1;
}

int loadFile(char *filename) {
  pc = text - memory;
  FILE *file = fopen(filename, "r");
  if (!file) {
    return -1;
  }
  fread((void *) text, 57444, 1, file);
  if (ferror(file)) {
    fclose(file);
    return -1;
  }
  fclose(file);
  return 0;
}
