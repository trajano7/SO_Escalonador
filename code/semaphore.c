#include <errno.h>
#include <stdio.h>
#include "semaphore.h"

// struct sembuf operacao[2]; 
// int idsem;
int p_sem(int idsem) {
    operacao[0].sem_num = 0;
    operacao[0].sem_op = 0;
    operacao[0].sem_flg = 0;
    operacao[1].sem_num = 0;
    operacao[1].sem_op = 1;
    operacao[1].sem_flg = 0;
    if(semop(idsem, operacao, 2) < 0) {
      printf("erro no p=%d\n", errno);
    }
}

int v_sem(int idsem) {
    operacao[0].sem_num = 0;
    operacao[0].sem_op = -1;
    operacao[0].sem_flg = 0;
    if(semop(idsem, operacao, 1) < 0) 
    printf("erro no p=%d\n", errno);
}