#include "vm.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
    return -1;
  }
  if (loadFile(argv[1])) {
    fprintf(stderr, "Could not load file %s\n", argv[1]);
    return -1;
  }
  while(execute(fetch()));
}
