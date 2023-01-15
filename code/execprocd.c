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
#include <sys/sem.h>

#include "list.h"
#include "semaphore.h"

/*
 cc -I. execprocd.c -o execprocd -Wall -Wextra -Werror -pedantic -g -O0 -M -MT
 dep/execprocd.d -MT bin/execprocd.o -MP -MF dep/execprocd.d -ggdb -DDEBUG && cc
 -I. execprocd.c -c -Wall -pedantic -Wextra -Werror -ggdb -O0 -DDEBUG -g -o
 bin/execprocd.o && cc -o execprocd bin/execprocd.o bin/list.o -Wall -pedantic
 -Wextra -Werror -ggdb -O0 -DDEBUG -g
*/

void terminaExecprocdHandler();
void exterminate(int, int, int, int);
void checkProcs(int, ProcList *procLists[3]);
// void checkProcs(int, ProcList*, ProcList*, ProcList*);

int signalType = -1;

// 1 algoritmo de escalonar
// 2 adicionar os fields pra reconhecer o estado do processo
// 3 cancela processo
// 4 terminar o termina processo
// 
int main()
{
  ProcList *procList = createList();
  int isRunning = 1;
  int killQueueKey = 0x6B69696C;
  int killQueueId;
  int procShmId;
  int procShmKey = 0x706964;
  int procQueueKey = 0x70726F63;
  int procQueueId;
  int idsem;

  struct SharedMem
  {
    long lastPid;
    int execprocdID;
    int endExecprocd;
  };
  struct SharedMem sharedMem;
  struct SharedMem *shmPointer;
  sharedMem.execprocdID = getpid();
  sharedMem.lastPid = 0;
  sharedMem.endExecprocd = 0;

  signal(SIGUSR1, terminaExecprocdHandler);

  if ((idsem = semget(0x73656d, 1, IPC_CREAT|0x1ff)) < 0) { 
       printf("erro na criacao do semaforo\n"); exit(1);
  }

  // Create proc shared memory
  if ((procShmId = shmget(procShmKey, sizeof(struct SharedMem), IPC_CREAT | 0777)) < 0)
  {
    printf("execprocd:\n");
    printf("Error creating shared memory %d!\n", procShmKey);
    return 1;
  }
  shmPointer = (struct SharedMem *)shmat(procShmId, (char *)0, 0);
  if (shmPointer == (struct SharedMem *)-1)
  {
    printf("execprocd:\n");
    printf("Error in attach!\n");
    return 1;
  }
  *shmPointer = sharedMem;

  // Create proc queue
  if ((procQueueId = msgget(procQueueKey, IPC_CREAT | 0777)) < 0)
  {
    printf("execprocd:\n");
    printf("Error getting queue %d!\n", procQueueKey);
    return 1;
  }

  ProcList *lowPriority = createList();
  ProcList *mediumPriority = createList();
  ProcList *highPriority = createList();
  ProcList *procLists[3] = {lowPriority, mediumPriority, highPriority};

  // TODO: excluir esse TODO
  // struct msqid_ds buf2;
  // msgctl(procQueueId, IPC_STAT, &buf2);
  // int msgQueueSize2 = buf2.msg_qnum;
  // printf("queueID = %d, msgQueueSize = %d\n", procQueueId, msgQueueSize2);

  while (isRunning)
  {
    // TODO checkProcs ta fudidao!
    // checkProcs(procQueueId, lowPriority, mediumPriority, highPriority);
    checkProcs(procQueueId, procLists);
    for (volatile unsigned i = 0; i < INT_MAX; i++)
    {
    }
    printf("execprocd just left busy waiting!\n");
    // alarm(10);
    // wait();
    p_sem(idsem);
    if (shmPointer->endExecprocd == 1)
    {
      // If kill was called, isRunning = false
      exterminate(killQueueId, procQueueId, procShmId, idsem);
      isRunning = 0;
    }
    v_sem(idsem);
  }

  printf("execprocd terminated!\n");
  printf("terminar processos aqui!\n");
  printf("escreve relatorio aqui!\n");

  freeList(procList);

  return 0;
}

void terminaExecprocdHandler()
{

  printf("RAPADURA com loló\n");
  signalType = 0;
}

void exterminate(int killQueueId, int procQueueId, int procShmId, int idsem)
{
  // Kill proc queue
  msgctl(procQueueId, IPC_RMID, NULL);
  // Kill proc shared memory
  shmctl(procShmId, IPC_RMID, NULL);
  // Remove semaphore
  if((semctl(idsem,0,IPC_RMID,0))==-1) {
    printf("exterminate: semctl error\n");
  }
}

// void checkProcs(int queueId, ProcList *lowPriority, ProcList *mediumPriority, ProcList *highPriority)
void checkProcs(int queueId, ProcList *procLists[3])
{
  struct MsgContent
  {
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
  struct msqid_ds buf2;
  unsigned msgQueueSize;
  Item *proc;

  // Get message queue size
  msgctl(queueId, IPC_STAT, &buf2);
  msgQueueSize = buf2.msg_qnum;
  printf("buf2.msg_qnum = %ld\n", buf2.msg_qnum);
  for (unsigned i = 0; i < msgQueueSize; i++)
  {
    int erro = msgrcv(queueId, &message, sizeof(message) - sizeof(long), 1, 0);
    if (erro == -1)
    {
      if (E2BIG == errno)
        printf("E2BIG");
      if (EACCES == errno)
        printf("EACCES");
      if (EIDRM == errno)
        printf("EIDRM");
      if (EINTR == errno)
        printf("EINTR");
      if (EINVAL == errno)
        printf("EINVAL");
      if (ENOMSG == errno)
        printf("ENOMSG");
      return;
    }
    printf("pid virtual = %s\n", message.msgContent.programName);
    printf("pid virtual = %d\n", message.msgContent.pidVirtual);
    printf("priority = %d\n", message.msgContent.priority);
    // printf("param = %s\n", message.msgContent.params[0]);

    proc = createItem(
      message.msgContent.pidVirtual,
      message.msgContent.priority,
      message.msgContent.programName,
      NULL
    );

    printf("PAROU AQUI0?\n");
    printf("%d\n", procLists[message.msgContent.priority]->lenght);
    printf("PAROU AQUI1?\n");

    pushBack(procLists[message.msgContent.priority], proc);
    printf("PAROU AQUI2?\n");
    printf("lenght = %d, procID = %d\n",
           procLists[message.msgContent.priority]->lenght,
           procLists[message.msgContent.priority]->first->item->pidVirtual);
    printf("PAROU AQUI3?\n");
  }
}
