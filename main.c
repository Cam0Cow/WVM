#include "vm.h"

int main(void) {
  if (loadFile("test.bin")) {
    printf("error\n");
  }
  execute(fetch());
  execute(fetch());
  execute(fetch());
  execute(fetch());
}
