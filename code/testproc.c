#include <limits.h>
#include <stdio.h>
#include <time.h>

/*
cc -I. testproc.c -o testproc -Wall -Wextra -pedantic -g -O0 -M -MT \
dep/testproc.d -MT bin/testproc.o -MP -MF dep/testproc.d -ggdb -DDEBUG &&
cc -I. testproc.c -c -Wall -pedantic -Wextra -ggdb -O0 -DDEBUG -g -o \
bin/testproc.o
cc -o testproc bin/testproc.o -Wall -pedantic -Wextra -ggdb -O0 -DDEBUG -g
*/

int main() {
  for (unsigned int j = 0; j < 10; j++) {
    for (unsigned int i = 0; i < INT_MAX; i++) {}
  }
  return 0;
}
