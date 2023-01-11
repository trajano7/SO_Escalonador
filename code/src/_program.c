#include <stdio.h>

#include "test.h"

int main() {
  if (test() == 0) {
    printf("and now I'm in main!\n");
  }
  return 0;
}
