#include <errno.h>
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

#include "list.h"
#include "semaphore.h"

/*
cc -I. execproc.c -o execproc -Wall -Wextra -pedantic -g -O0 -M -MT \
dep/execproc.d -MT bin/execproc.o -MP -MF dep/execproc.d -ggdb -DDEBUG &&
cc -I. execproc.c -c -Wall -pedantic -Wextra -ggdb -O0 -DDEBUG -g -o \
bin/execproc.o &&
cc -o execproc bin/execproc.o bin/list.o bin/semaphore.o -Wall -pedantic \
-Wextra -ggdb -O0 -DDEBUG -g
*/

int main(int argc, char *argv[]) {
  int pid;
  int queueId;
  int shmKey = 0x706964;
  int queueKey = 0x70726F63;
  int shmId;
  int semaphoreKey = 0x73656d;
  int idsem;
  struct SharedMem {
    long lastPid;
    int execprocdID;
    int cancelProcID;  // REF#
  };
  struct SharedMem *shmPointer;
  struct MsgContent {
    char programName[30];
    long pidVirtual;
    int priority;
  };
  struct Message {
    long type;
    struct MsgContent msgContent;
  };
  struct Message message;

  if (argc < 3) {
    printf("execproc\n");
    printf("Error - too few arguments!\n");
    printf("At least proc name and priority are needed:\n");
    printf("  ./execproc <procName> <priority> <arg1> <arg2>...\n");
    return 1;
  }

  if ((idsem = semget(semaphoreKey, 1, 0x1ff)) < 0) {
    printf("erro na criacao do semaforo\n");
    exit(1);
  }

  if ((shmId = shmget(shmKey, sizeof(struct SharedMem), 0x1ff)) < 0) {
    printf("execproc\n");
    printf("Error getting shared memory %d!\n", shmKey);
    return 1;
  }
  shmPointer = (struct SharedMem *)shmat(shmId, (char *)0, 0);
  if (shmPointer == (struct SharedMem *)-1) {
    printf("execproc\n");
    printf("Error in attach!\n");
    return 1;
  }

  p_sem(idsem);
  pid = shmPointer->lastPid;
  shmPointer->lastPid = pid + 1;
  v_sem(idsem);

  // Get proc queue
  if ((queueId = msgget(queueKey, 0777)) < 0) {
    printf("execproc:\n");
    printf("Error getting queue %d!\n", queueKey);
    return 1;
  }

  message.type = 1;
  message.msgContent.pidVirtual = pid;
  message.msgContent.priority = atoi(argv[2]);
  strcpy(message.msgContent.programName, argv[1]);

  // Send proc request
  if (msgsnd(queueId, &message, sizeof(message) - sizeof(long), 0) == -1)
    printf("Deu erro no envio");

  shmdt(shmPointer);

  return 0;
}
