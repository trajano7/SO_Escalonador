#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
  // int queueId;
  // int queueKey = 0x6B69696C;
  int shmKey = 0x706964;
  int shmId;
  int execProcdPID;
  struct SharedMem {
    long lastPid;
    int execprocdID;
    int endExecprocd;
  };
  struct SharedMem* shmPointer;
  struct KillMsg {
    long pid;
    int kill;
  } killMsg;

  if ((shmId = shmget(shmKey, sizeof(struct SharedMem), 0777)) < 0) {
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
  execProcdPID = shmPointer->execprocdID;
  printf("execprocdPID = %d, lastPid = %ld\n", execProcdPID, shmPointer->lastPid);

  shmPointer->endExecprocd = 1;
  kill(execProcdPID,SIGUSR1);

  return 0;
  
}
