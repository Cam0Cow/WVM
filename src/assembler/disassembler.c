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
  "ROTC"
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
    if (code[pc] == 2) {
      printf("%s %d\n", opcodes[code[pc]], code[pc+1]);
      pc +=2;
    } else if (code[pc] == 3) {
      printf("%s %d\n", opcodes[code[pc]], (code[pc+2] << 8) | code[pc+1]);
      pc += 3;
    } else {
      printf("%s\n", opcodes[code[pc]]);
      pc++;
    }
  }
}
