#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  int priority = atoi(argv[2]);

  printf("Priority = %d\n", priority);

  execl(argv[1], argv[1], (char *)NULL);

  printf("Error - returned to execl in execproc!\n");
  return 0;
}
