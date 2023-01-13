#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  if (argc < 3) {
    printf("Error - too few arguments!\n");
    printf("At least proc name and priority are needed:\n");
    printf("  ./execproc <procName> <priority> <arg1> <arg2>...\n");
    return 0;
  }

  printf("Priority = %d\n", atoi(argv[2]));

  execl(argv[1], argv[1], NULL);

  printf("Error - returned from execl in execproc!\n");
  return 0;
}
