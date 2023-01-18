#include <stdio.h>
#include <time.h>

/*
cc -I . testproc2.c -o testproc2 -Wall -Wextra -pedantic -g -O0 -M -MT \
dep/testproc2.d -MT bin/testproc2.o -MP -MF dep/testproc2.d -ggdb -DDEBUG &&
cc -I. testproc2.c -c -Wall -pedantic -Wextra -ggdb -O0 -DDEBUG -g -o \
bin/testproc2.o &&
cc -o testproc2 bin/testproc2.o -Wall -pedantic -Wextra -ggdb -O0 -DDEBUG -g
*/

int main() {
  volatile unsigned i;
  volatile unsigned j;
  time_t begin;
  time_t total;
  printf("[testproc2] BEGIN\n");
  begin = time(NULL);
  for (i = 0; i < 8; i++) {
    for (j = 0; j < 2100000000; j++) {}
    printf("[testproc2] LOOP\n");
    printf("  i = %d\n", i);
  }
  printf("[testproc2] TIME\n");
  total = time(NULL) - begin;
  printf("  %ld seconds\n", total);
  printf("[testproc2] END\n");
  return total;
}
