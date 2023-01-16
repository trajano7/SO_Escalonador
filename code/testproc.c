#include <limits.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
  // printf("Filename: %s\n", argv[0]);

  // printf("Inside %s!\n", argv[0]);
  printf("PROCESSO FILHO\n");

  for (volatile unsigned i = 0; i < INT_MAX; i++) {
    for (volatile unsigned i = 0; i < INT_MAX; i++) {
      for (volatile unsigned i = 0; i < INT_MAX; i++) {}
    }
  }

  return 0;
}
