#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/sem.h>

#include "semaphore.h"
#include "list.h"

int main(int argc, char *argv[])
{
  int pid;
  int queueId;
  int shmKey = 0x706964;
  int queueKey = 0x70726F63;
  int shmId;
  int idsem;
  // int *shmPointer;
  struct SharedMem {
    long lastPid;
    int execprocdID;
    int endExecprocd;
  };
  struct SharedMem* shmPointer;
  struct MsgContent {
    char programName[30];
    int pidVirtual;
    int priority;
  } msgContent;
  struct Message
  {
    long type;
    struct MsgContent msgContent;
  };
  struct Message message;

  if (argc < 3)
  {
    printf("execproc\n");
    printf("Error - too few arguments!\n");
    printf("At least proc name and priority are needed:\n");
    printf("  ./execproc <procName> <priority> <arg1> <arg2>...\n");
    return 1;
  }

  if ((idsem = semget(0x73656d, 1, 0x1ff)) < 0) { 
       printf("erro na criacao do semaforo\n"); exit(1);
  }

  if ((shmId = shmget(shmKey, sizeof(struct SharedMem), 0x1ff)) < 0) {
    printf("execproc\n");
    printf("Error getting shared memory %d!\n", shmKey);
    return 1;
  }
  shmPointer = (struct SharedMem*)shmat(shmId, (char *)0, 0);
  if (shmPointer == (struct SharedMem*)-1) {
    printf("execproc\n");
    printf("Error in attach!\n");
    return 1;
  }

  p_sem(idsem);
  pid = shmPointer->lastPid;
  shmPointer->lastPid = pid + 1;
  v_sem(idsem);

  // Get proc queue
  if ((queueId = msgget(queueKey, 0777)) < 0)
  {
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

  return 0;
}
