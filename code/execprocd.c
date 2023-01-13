#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "list.h"

/*
 cc -I. execprocd.c -o execprocd -Wall -Wextra -Werror -pedantic -g -O0 -M -MT
 dep/execprocd.d -MT bin/execprocd.o -MP -MF dep/execprocd.d -ggdb -DDEBUG && cc
 -I. execprocd.c -c -Wall -pedantic -Wextra -Werror -ggdb -O0 -DDEBUG -g -o
 bin/execprocd.o && cc -o execprocd bin/execprocd.o bin/list.o -Wall -pedantic
 -Wextra -Werror -ggdb -O0 -DDEBUG -g
*/

int checkKill(int, int, int);
void checkProcs(int);

int main() {
  ProcList* procList = createList();
  int isRunning = 1;
  int killQueueKey = 0x6B69696C;
  int killQueueId;
  int procShmId;
  int* shmPointer;
  int procShmKey = 0x706964;
  int procQueueKey = 0x70726F63;
  int procQueueId;

  // Create kill queue
  if ((killQueueId = msgget(killQueueKey, IPC_CREAT | 0777)) < 0) {
    printf("execprocd:\n");
    printf("Error getting queue %d!\n", killQueueKey);
    return 1;
  }

  // Create proc shared memory
  if ((procShmId = shmget(procShmKey, sizeof(int), IPC_CREAT | 0777)) < 0) {
    printf("execprocd:\n");
    printf("Error creating shared memory %d!\n", procShmKey);
    return 1;
  }
  shmPointer = (int*)shmat(procShmId, (char*)0, 0);
  if (shmPointer == (int*)-1) {
    printf("execprocd:\n");
    printf("Error in attach!\n");
    return 1;
  }
  *shmPointer = 0;

  // Create proc queue
  if ((procQueueId = msgget(procQueueKey, IPC_CREAT | 0777)) < 0) {
    printf("execprocd:\n");
    printf("Error getting queue %d!\n", procQueueKey);
    return 1;
  }

  while (isRunning) {
    // If kill was called, isRunning = false
    isRunning = checkKill(killQueueId, procQueueId, procShmId);
    // TODO checkProcs ta fudidao!
    checkProcs(procQueueId);
    for (volatile unsigned i = 0; i < INT_MAX; i++) {}
    printf("execprocd just left busy waiting!\n");
  }

  printf("execprocd terminated!\n");
  printf("terminar processos aqui!\n");
  printf("escreve relatorio aqui!\n");

  freeList(procList);

  return 0;
}

int checkKill(int killQueueId, int procQueueId, int procShmId) {
  struct KillMsg {
    int kill;
  } killMsg;
  struct msqid_ds buf;
  unsigned msgQueueSize;
  // Get message queue size
  msgctl(killQueueId, IPC_STAT, &buf);
  msgQueueSize = buf.msg_qnum;
  // Check message queue size
  if (msgQueueSize > 0) {
    // Get kill msg (struct has kill == 1 if wants to kill)
    msgrcv(killQueueId, &killMsg, sizeof(killMsg), 0, 0);
    if (killMsg.kill == 1) {
      // Kill kill queue
      msgctl(killQueueId, IPC_RMID, NULL);
      printf("cheguei aqui\n");
      // Kill proc queue
      msgctl(procQueueId, IPC_RMID, NULL);
      // Kill proc shared memory
      shmctl(procShmId, IPC_RMID, NULL);
      return 0;
    }
    return 1;
  }
  return 1;
}

void checkProcs(int queueId) {
  Item* proc;
  struct msqid_ds buf;
  unsigned msgQueueSize;
  // Get message queue size
  msgctl(queueId, IPC_STAT, &buf);
  msgQueueSize = buf.msg_qnum;
  printf("buf.msg_qnum = %ld\n", buf.msg_qnum);
  for (unsigned i = 0; i < msgQueueSize; i++) {
    msgrcv(queueId, &proc, sizeof(proc), 0, 0);
    printf("execprocd:\n");
    if (proc == NULL) {
      printf("merda\n");
      return;
    }
    printf("New process! pid = %d, priority = %d\n", proc->pid, proc->priority);
  }
}
