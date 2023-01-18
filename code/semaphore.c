#include "semaphore.h"

#include <errno.h>
#include <stdio.h>

/*
cc -I. semaphore.c -o semaphore -Wall -Wextra -pedantic -g -O0 -M -MT \
dep/semaphore.d -MT bin/semaphore.o -MP -MF dep/semaphore.d -ggdb -DDEBUG &&
cc -I. semaphore.c -c -Wall -pedantic -Wextra -ggdb -O0 -DDEBUG -g -o \
bin/semaphore.o
*/

struct sembuf operacao[2];
// int idsem;
int p_sem(int idsem) {
  operacao[0].sem_num = 0;
  operacao[0].sem_op = 0;
  operacao[0].sem_flg = 0;
  operacao[1].sem_num = 0;
  operacao[1].sem_op = 1;
  operacao[1].sem_flg = 0;
  if (semop(idsem, operacao, 2) < 0) {
    printf("erro no p=%d\n", errno);
    return 1;
  }
  return 0;
}

int v_sem(int idsem) {
  operacao[0].sem_num = 0;
  operacao[0].sem_op = -1;
  operacao[0].sem_flg = 0;
  if (semop(idsem, operacao, 1) < 0) {
    printf("erro no p=%d\n", errno);
    return 1;
  }
  return 0;
}
