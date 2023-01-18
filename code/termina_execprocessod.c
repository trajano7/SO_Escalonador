#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "semaphore.h"

/*
cc -I. termina_execprocessod.c -o termina_execprocessod -Wall -Wextra \
-pedantic -g -O0 -M -MT dep/termina_execprocessod.d -MT \
bin/termina_execprocessod.o -MP -MF dep/termina_execprocessod.d -ggdb -DDEBUG &&
cc -I. termina_execprocessod.c -c -Wall -pedantic -Wextra -ggdb -O0 -DDEBUG -g \
-o bin/termina_execprocessod.o &&
cc -o termina_execprocessod bin/termina_execprocessod.o bin/semaphore.o -Wall \
-pedantic -Wextra -ggdb -O0 -DDEBUG -g
*/

int main() {
  int shmKey = 0x706964;
  int shmId;
  int semaphoreKey = 0x73656d;
  int idsem;
  int execProcdPID;
  struct SharedMem {
    long lastPid;
    int execprocdID;
    int cancelProcID;  // REF#
  };
  struct SharedMem* shmPointer;
  struct KillMsg {
    long pid;
    int kill;
  };

  if ((idsem = semget(semaphoreKey, 1, 0x1ff)) < 0) {
    printf("erro na criacao do semaforo\n");
    exit(1);
  }

  if ((shmId = shmget(shmKey, sizeof(struct SharedMem), 0777)) < 0) {
    printf("execproc\n");
    printf("Error getting shared memory %d!\n", shmKey);
    return 1;
  }
  shmPointer = (struct SharedMem*)shmat(shmId, (char*)0, 0);
  if (shmPointer == (struct SharedMem*)-1) {
    printf("execproc\n");
    printf("Error in attach!\n");
    return 1;
  }
  p_sem(idsem);
  execProcdPID = shmPointer->execprocdID;
  v_sem(idsem);
  kill(execProcdPID, SIGUSR1);

  return 0;
}
