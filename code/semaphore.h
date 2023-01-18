#include<sys/sem.h>

extern struct sembuf operacao[2]; 

int p_sem(int idsem);
int v_sem(int idsem);