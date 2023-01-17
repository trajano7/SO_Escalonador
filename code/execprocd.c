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
#include <time.h>
#include <stdlib.h>

#include "list.h"
#include "semaphore.h"

#define STATIC 0
#define DYNAMIC 1
#define RANDOM 2

/*
 cc -I. execprocd.c -o execprocd -Wall -Wextra -Werror -pedantic -g -O0 -M -MT
 dep/execprocd.d -MT bin/execprocd.o -MP -MF dep/execprocd.d -ggdb -DDEBUG && cc
 -I. execprocd.c -c -Wall -pedantic -Wextra -Werror -ggdb -O0 -DDEBUG -g -o
 bin/execprocd.o && cc -o execprocd bin/execprocd.o bin/list.o -Wall -pedantic
 -Wextra -Werror -ggdb -O0 -DDEBUG -g
*/

void terminaExecprocdHandler();
void cancelProcHandler();
void alarmHandler();
int allEmpty(ProcList *procLists[3]);
void exterminate(int, int, int);
void checkProcs(int, ProcList *procLists[3]);
void printProcessStatus(Item *proc);
Item *scheduler(ProcList *procLists[3], int *totalQuantum, int *totalContext, int schedulerMode);
// void checkProcs(int, ProcList*, ProcList*, ProcList*);

// 1 algoritmo de escalonar
// 2 adicionar os fields pra reconhecer o estado do processo
// 3 cancela processo
// 4 terminar o termina processo

int endExecprocd = 0; //Flag that indicates if execprocd has to end, set by termina_processod
int cancelProc = 0; //Flag that indicates if a cancel_proc happend

int main()
{
  ProcList *procList = createList();
  int procShmId, procQueueId, idsem, newProcPid, childReturn;
  int totalContext = 0;
  int totalQuantum = 0;
  int procShmKey = 0x706964;
  int procQueueKey = 0x70726F63;
  int semaphoreKey = 0x73656d;
  int schedulerMode = STATIC;

  Item *runningProc = NULL; //Current running process

  struct SharedMem
  {
    long lastPid; //Last virtual PID, used in execproc
    int execprocdID; //execprocd PID, used to send signals to execproc
    // int endExecprocd; //Flag that indicates if execprocd has to end, set by termina_processod
    int cancelProcID; //REF#
  };
  struct SharedMem sharedMem;
  struct SharedMem *shmPointer;
  sharedMem.execprocdID = getpid();
  sharedMem.lastPid = 0;
  // sharedMem.endExecprocd = 0;
  sharedMem.cancelProcID = -1;

  signal(SIGUSR1, terminaExecprocdHandler);
  signal(SIGUSR2, cancelProcHandler);
  signal(SIGALRM, alarmHandler);

  //Config SIGALARM and SIGACTION to unlock the wait after a interruption
  struct sigaction info;
  sigaction(SIGALRM, NULL, &info);
  info.sa_flags = 0;
  sigaction(SIGALRM, &info, NULL);
  sigaction(SIGUSR1, NULL, &info);
  info.sa_flags = 0;
  sigaction(SIGUSR1, &info, NULL);
  sigaction(SIGUSR2, NULL, &info);
  info.sa_flags = 0;
  sigaction(SIGUSR2, &info, NULL);

  if ((idsem = semget(semaphoreKey, 1, IPC_CREAT | 0x1ff)) < 0) {
    printf("Error creating semaphore %d!\n", semaphoreKey);
    exit(1);
  }

  // Create proc shared memory
  if ((procShmId = shmget(procShmKey, sizeof(struct SharedMem), IPC_CREAT | 0777)) < 0) {
    printf("execprocd:\n");
    printf("Error creating shared memory %d!\n", procShmKey);
    return 1;
  }
  shmPointer = (struct SharedMem *)shmat(procShmId, (char *)0, 0);
  if (shmPointer == (struct SharedMem *)-1) {
    printf("execprocd:\n");
    printf("Error in attach!\n");
    return 1;
  }
  *shmPointer = sharedMem;

  // Create process message queue
  if ((procQueueId = msgget(procQueueKey, IPC_CREAT | 0777)) < 0) {
    printf("execprocd:\n");
    printf("Error getting queue %d!\n", procQueueKey);
    return 1;
  }

  //Create ready queues
  ProcList *lowPriority = createList();
  ProcList *mediumPriority = createList();
  ProcList *highPriority = createList();
  ProcList *procLists[3] = {highPriority, mediumPriority, lowPriority};

  while (1) {
    checkProcs(procQueueId, procLists);
    runningProc = scheduler(procLists, &totalQuantum, &totalContext, schedulerMode);

    if (runningProc != NULL)
    {
      if (runningProc->quantumTimes == 0)
      {
        if ((newProcPid = fork()) < 0)
        {
          printf("erro no fork\n");
          exit(1);
        }
        if (newProcPid == 0)
        {
          if (execl(runningProc->programName, runningProc->programName, (char *)0) == -1)
          {
            printf("erro no execl para o programa %s\n", runningProc->programName);
          }
        }
        runningProc->pidReal = newProcPid;
      }
      else
      {
        kill(runningProc->pidReal, SIGCONT);
      }
      runningProc->quantumTimes++;

      if (!endExecprocd && !cancelProc) {
        alarm(10);
        wait(&childReturn);
      }
      if (alarm(0) != 0 && !endExecprocd && !cancelProc) {
        printProcessStatus(runningProc);
      }
      else
      {
        kill(runningProc->pidReal, SIGTSTP);
        checkProcs(procQueueId, procLists);
        pushBack(procLists[runningProc->priority], runningProc);
      }
      runningProc = NULL;
    }

    if (endExecprocd == 1) {
      // If kill was called, isRunning = false
      exterminate(procQueueId, procShmId, idsem);
      break;
    }
    if (cancelProc == 1) {
      p_sem(idsem);
      int cancelProcPID = shmPointer->cancelProcID;
      v_sem(idsem);
      printf("cancel proc PID = %d\n", cancelProcPID);
      cancelProc = 0;
    }
  }

  ListItem* aux;

  for (int i = 0; i < 3; i++) {
    if (procLists[i]->lenght != 0) {
      aux = procLists[i]->first;
      for (int j = 0; j < procLists[i]->lenght; j++) {
        if(aux->item->quantumTimes > 0) kill(aux->item->pidReal,SIGKILL);
        aux = aux->right;
      }
    }
  }

  if (runningProc != NULL) {
    printf("killing running process\n");
    kill(runningProc->pidReal, SIGKILL);
  }
  printf("execprocd terminated!\n");
  printf("terminar processos aqui!\n");
  printf("escreve relatorio aqui!\n");

  freeList(procList);

  return 0;
}

void alarmHandler()
{
  return;
}

void terminaExecprocdHandler()
{
  endExecprocd = 1;
}

void cancelProcHandler() {
  cancelProc = 1;
}

int allEmpty(ProcList *procLists[3])
{
  int empty = 1;

  ListItem *auxProc;
  for (int i = 0; i < 3; i++)
  {
    if (procLists[i]->lenght != 0)
    {
      empty = 0;
      break;
    }
  }

  return empty;
}

void printProcessStatus(Item *proc)
{
  printf("Processo %s terminou de executar\n", proc->programName);
  printf("Tempo de turnaround: %ld\n", (time(NULL) - proc->startTime)/3600);
  printf("Trocas de contexto: %d\n", proc->quantumTimes);
  printf("Pid: %d\n", proc->pidVirtual);
}

Item *scheduler(ProcList *procLists[3], int *totalQuantum, int *totalContext, int schedulerMode)
{

  Item *runningProc;
  int randomProc;

  if(allEmpty(procLists)) return NULL;

  if (schedulerMode == STATIC || schedulerMode == DYNAMIC)
  {

    if (schedulerMode == DYNAMIC)
    {
      // incrementa tempo sem running
      ListItem *auxProc;
      for (int i = 0; i < 3; i++)
      {
        auxProc = procLists[i]->first;
        for (unsigned int j = 0; j < procLists[i]->lenght; j++)
        {
          auxProc->item->dynamicCriteria++;
          auxProc = auxProc->right;
        }
      }

      // altera prioridade
      if (*totalQuantum % 6 == 0)
      {
        ListItem *auxProc;
        for (int i = 0; i < 3; i++)
        {
          auxProc = procLists[i]->first;
          for (unsigned int j = 0; j < procLists[i]->lenght; j++)
          {
            // alta prioridade
            if (i == 0)
            {
              if (auxProc->item->dynamicCriteria >= 3)
              {
                Item *auxItem = popItem(procLists[0], j);
                auxItem->priority = 1;
                auxProc->item->dynamicCriteria = 0;
                pushBack(procLists[1], auxItem);
              }
            }
            // media prioridade
            if (i == 1)
            {
              if (auxProc->item->dynamicCriteria >= 3)
              {
                Item *auxItem = popItem(procLists[1], j);
                auxItem->priority = 2;
                auxProc->item->dynamicCriteria = 0;
                pushBack(procLists[2], auxItem);
              }
              else if (auxProc->item->dynamicCriteria < 3)
              {
                Item *auxItem = popItem(procLists[1], j);
                auxItem->priority = 0;
                auxProc->item->dynamicCriteria = 0;
                pushBack(procLists[0], auxItem);
              }
            }
            // baixa prioridade
            if (i == 2)
            {
              if (auxProc->item->dynamicCriteria < 3)
              {
                Item *auxItem = popItem(procLists[0], j);
                auxItem->priority = 1;
                auxProc->item->dynamicCriteria = 0;
                pushBack(procLists[1], auxItem);
              }
            }
            auxProc->item->dynamicCriteria++;
            auxProc = auxProc->right;
          }
        }
      }
    }

    if (procLists[0]->lenght)
    {
      runningProc = popFront(procLists[0]);
    }
    else if (procLists[1]->lenght)
    {
      runningProc = popFront(procLists[1]);
    }
    else if (procLists[2]->lenght)
    {
      runningProc = popFront(procLists[2]);
    }
  }
  else
  {
    int totalLenght = procLists[0]->lenght + procLists[1]->lenght + procLists[2]->lenght;
    srand(time(NULL));
    int randomNum = rand() % (totalLenght);
    if (randomNum < procLists[0]->lenght)
    {
      runningProc = popItem(procLists[0], randomNum);
    }
    else if (randomNum >= procLists[0]->lenght && randomNum < (totalLenght - procLists[2]->lenght))
    {
      runningProc = popItem(procLists[1], randomNum-procLists[0]->lenght);
    }
    else if (randomNum >= procLists[0]->lenght + procLists[1]->lenght)
    {
      runningProc = popItem(procLists[2], randomNum-(procLists[0]->lenght + procLists[1]->lenght));
    }
  }
  // runningProc->quantumTimes += 1;

  *totalQuantum++;
  printf("Processo %d (%d) escalonado.\n", runningProc->pidVirtual, runningProc->pidReal);
  return runningProc;
}

void exterminate(int procQueueId, int procShmId, int idsem)
{
  // Kill proc queue
  if ((msgctl(procQueueId, IPC_RMID, NULL)) == -1) {
    printf("Message queue remove erro.\n");
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
  }
  // Kill proc shared memory
  if ((shmctl(procShmId, IPC_RMID, NULL)) == -1) {
    printf("Shared memory remove erro.\n");
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
  }
  // Remove semaphore
  if ((semctl(idsem, 0, IPC_RMID, 0)) == -1)
  {
    printf("Semaphore remove erro.\n");
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
  }
}

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

    proc = createItem(
        message.msgContent.pidVirtual,
        message.msgContent.priority,
        message.msgContent.programName,
        NULL);

    /*
    LISTA DE ITEMS DENTRO DO LIST ITEM:
      + PIDVIRTUAL #
      + PIDREAL #
      + PRIORITY #
      + NOME DO PROGRAMA #
      + PARAMS # -> NAO USANDO
      + NUMERO DE VEZES ESCALONADO
      + TEMPO DO PRIMEIRO ESCALONAMENTO
      + QUANTUM TIMES VEZES QUE O QUANTUM OCORREU COM O PROCESSO EM READY
    */

    pushBack(procLists[message.msgContent.priority], proc);

    printf("lenght = %d, procID = %d, name = %s\n",
           procLists[message.msgContent.priority]->lenght,
           procLists[message.msgContent.priority]->first->item->pidVirtual,
           procLists[message.msgContent.priority]->first->item->programName);
    printf("Item adicionado na lista de prioridade %d:\n", message.msgContent.priority);
    printAll(procLists[message.msgContent.priority]);
    printf("-------------------------\n");
  }
}
