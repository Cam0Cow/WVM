#include "vm.h"

int main(void) {
  if (loadFile("test.bin")) {
    return -1;
  }
  while(execute(fetch()));
}
