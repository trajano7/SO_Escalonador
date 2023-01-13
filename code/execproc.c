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

#include "list.h"

int main(int argc, char *argv[]) {
  int pid;
  int queueId;
  int shmKey = 0x706964;
  int queueKey = 0x70726F63;
  int shmId;
  int *shmPointer;

  if (argc < 3) {
    printf("execproc\n");
    printf("Error - too few arguments!\n");
    printf("At least proc name and priority are needed:\n");
    printf("  ./execproc <procName> <priority> <arg1> <arg2>...\n");
    return 1;
  }
  if ((shmId = shmget(shmKey, sizeof(int), 0)) < 0) {
    printf("execproc\n");
    printf("Error getting shared memory %d!\n", shmKey);
    return 1;
  }
  shmPointer = (int *)shmat(shmId, (char *)0, 0);
  if (shmPointer == (int *)-1) {
    printf("execproc\n");
    printf("Error in attach!\n");
    return 1;
  }
  pid = *shmPointer;
  *shmPointer = pid + 1;

  Item *procMsg = createItem(pid, atoi(argv[2]), NULL);  // NULL nao!

  // Get proc queue
  if ((queueId = msgget(queueKey, 0777)) < 0) {
    printf("execproc:\n");
    printf("Error getting queue %d!\n", queueKey);
    return 1;
  }

  // Send proc request
  msgsnd(queueId, &procMsg, sizeof(procMsg), 0);

  printf("Proc sent!\n");
  printf("pid = %d\n", procMsg->pid);
  printf("priority = %d\n", procMsg->priority);

  return 0;
}
