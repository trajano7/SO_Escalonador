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
void alarmHandler();
int allEmpty(ProcList *procLists[3]);
void exterminate(int, int, int, int);
void checkProcs(int, ProcList *procLists[3]);
Item* scheduler(ProcList *procLists[3], int *totalQuantum, int *totalContext, int schedulerMode);
// void checkProcs(int, ProcList*, ProcList*, ProcList*);

// 1 algoritmo de escalonar
// 2 adicionar os fields pra reconhecer o estado do processo
// 3 cancela processo
// 4 terminar o termina processo

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
  int totalQuantum = 0;
  int totalContext = 0;
  int schedulerMode = STATIC;
  int newProcPid;
  int childReturn;
  Item *runningProc = NULL;

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
  signal(SIGALRM, alarmHandler);

  struct sigaction info, new;
  sigaction(SIGALRM,NULL,&info);
  new = info;
  new.sa_flags = 0;
  sigaction(SIGALRM,&new,NULL);
  sigaction(SIGUSR1,NULL,&info);
  new = info;
  new.sa_flags = 0;
  sigaction(SIGUSR1,&new,NULL);

  if ((idsem = semget(0x73656d, 1, IPC_CREAT | 0x1ff)) < 0)
  {
    printf("erro na criacao do semaforo\n");
    exit(1);
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

    checkProcs(procQueueId, procLists);
    if (runningProc != NULL) {
      printf("ENTROU NO EXECUTADOR %d\n", runningProc->quantumTimes);
      if (runningProc->quantumTimes == 0) {
        printf("ENTROU NA PRIMEIRA EXECUCAO\n");
        if ((newProcPid = fork()) < 0) {
          printf("erro no fork\n"); exit(1); 
        } 
        if (newProcPid == 0) {
          printf("ENTROU NO EXECL\n");
          if(execl(runningProc->programName,runningProc->programName, (char *) 0) == -1) {
            printf("erro no execl para o programa %s\n", runningProc->programName);
          }
        }
        runningProc->pidReal = newProcPid;
        runningProc->quantumTimes++;
      }
      else {
        printf("ENTROU NO CONTINUE\n");
        kill(runningProc->pidReal,SIGCONT);
      }

      alarm(10);
      wait(&childReturn);
      if (alarm(0) != 0) {
        printf("PROCESSO FILHO DEU EXIT\n");
        runningProc = NULL;
      }
      else {
        kill(runningProc->pidReal,SIGTSTP);
        pushBack(procLists[runningProc->priority],runningProc);
        printf("PROCESSO FILHO PAUSADO\n");
      }
    }
    else {
      for (volatile unsigned i = 0; i < INT_MAX; i++) {}
      // printf("execprocd just left busy waiting!\n");
    }

    

    
    // for (volatile unsigned i = 0; i < INT_MAX; i++)
    // {
    // } 

    if (!allEmpty(procLists))
    {
      printf("picaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\n");
      printAll(lowPriority);
      printAll(mediumPriority);
      printAll(highPriority);
      printf("teste0 = %s\n", procLists[0]->first->item->programName);
      runningProc = scheduler(procLists, &totalQuantum, &totalContext, schedulerMode);
      printf("AQUIIIIIIIIIIIIIIIIIIIIIIIIIIIII: %d\n", runningProc->pidVirtual);
      printf("picaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa2222222222222222222222222222\n");
      if (runningProc != NULL) printf("EH DIFERENTE DE NULO!EH DIFERENTE DE NULO!EH DIFERENTE DE NULO!\n");
    }

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

  if (runningProc != NULL) kill(runningProc->pidReal, SIGKILL);
  printf("execprocd terminated!\n");
  printf("terminar processos aqui!\n");
  printf("escreve relatorio aqui!\n");

  freeList(procList);

  return 0;
}

void alarmHandler() {
  return;
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

Item* scheduler(ProcList *procLists[3], int *totalQuantum, int *totalContext, int schedulerMode)
{

  Item* runningProc;

  /*
    VARIAVEL DO EXECUTA PROCESSO D -> QUANTUMS QUE JA PASSARAM = 0 -> 1 -> 2 ->
    ultima vez que rodou = QUANTUMS QUE JA PASSARAM;
    QUANTUS QUE JA PASSARAM - ULTIMA VEZ QUE RODOU > NUM -> AUMENTA PRIORIDADE


  */

  // if (mode == DINAMIC)
  // {
  //   printf("CALCULANDO PRIORIDADE\n");
  //   procLists[0]->first->item->quantumTimes
  // }

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
    else if (randomNum >= procLists[0]->lenght && randomNum < totalLenght - procLists[2]->lenght)
    {
      runningProc = popItem(procLists[1], randomNum);
    }
    else if (randomNum >= procLists[0]->lenght + procLists[1]->lenght)
    {
      runningProc = popItem(procLists[2], randomNum);
    }
  }
  // runningProc->quantumTimes += 1;

  *totalQuantum++;
  printf("runningProc: %s\n", runningProc->programName);
  return runningProc;
}

void terminaExecprocdHandler()
{
  return;
}

void exterminate(int killQueueId, int procQueueId, int procShmId, int idsem)
{
  // Kill proc queue
  msgctl(procQueueId, IPC_RMID, NULL);
  // Kill proc shared memory
  shmctl(procShmId, IPC_RMID, NULL);
  // Remove semaphore
  if ((semctl(idsem, 0, IPC_RMID, 0)) == -1)
  {
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
  // printf("buf2.msg_qnum = %ld\n", buf2.msg_qnum);
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

    printf("%d\n", procLists[message.msgContent.priority]->lenght);

    pushBack(procLists[message.msgContent.priority], proc);
    printf("lenght = %d, procID = %d, name = %s\n",
           procLists[message.msgContent.priority]->lenght,
           procLists[message.msgContent.priority]->first->item->pidVirtual,
           procLists[message.msgContent.priority]->first->item->programName);
    printf("CELTA 2012\n");
  }
}
