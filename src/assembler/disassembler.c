#include <stdio.h>
#include <stdlib.h>

#define MAX_SIZE 57444

char code[MAX_SIZE];

char *opcodes[] = {
  "BRK",
  "NOP",
  "PUSHB",
  "PUSHC",
  "ADDB",
  "SUBB",
  "MULB",
  "DIVB",
  "ADDC",
  "SUBC",
  "MULC",
  "DIVC",
  "LOGB",
  "PUTB",
  "LOGC",
  "PUTC",
  "DUPB",
  "DROPB",
  "SWAPB",
  "ROTB",
  "DUPC",
  "DROPC",
  "SWAPC",
  "ROTC",
  "IN",
  "[WIP]",
  "JT",
  "JMP",
  "LDB",
  "STRB",
  "LDC",
  "STRC",
  "NOTB",
  "ANDB",
  "ORB",
  "XORB",
  "NOTC",
  "ANDC",
  "ORC",
  "XORC",
  "LTB",
  "LEB",
  "EQB",
  "GTB",
  "GEB",
  "LTC",
  "LEC",
  "EQC",
  "GTC",
  "GEC"
  "SEB",
  "UEB"
};

int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
    return -1;
  }
  FILE *file = fopen(argv[1], "rb");
  if (!file) {
    fprintf(stderr, "File does not exist\n");
    return -1;
  }
  int length = fread((void *) code, 1, MAX_SIZE, file);
  int pc = 0;
  while (pc < length) {
    printf("%d: ", pc + 8192);
    if (code[pc] == 2) {
      printf("%s %d\n", opcodes[code[pc]], code[pc+1]);
      pc += 2;
    } else if (code[pc] == 3) {
      printf("%s %d\n", opcodes[code[pc]], (code[pc+2] << 8) | code[pc+1]);
      pc += 3;
    } else if (code[pc] == 27) {
      printf("%s %d\n", opcodes[code[pc]], (code[pc+2] << 8) | code[pc+1]);
      pc += 3;
    } else if (code[pc] == 26) {
      int offset = code[pc+1];
      printf("%s %d (to %d)\n", opcodes[code[pc]], offset, offset+pc+8192);
      pc += 2;
    } else {
      printf("%s\n", opcodes[code[pc]]);
      pc++;
    }
  }
}
