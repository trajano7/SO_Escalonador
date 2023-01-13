#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
  int queueId;
  int queueKey = 0x6B69696C;
  struct KillMsg {
    int kill;
  } killMsg;

  // Get kill queue
  if ((queueId = msgget(queueKey, 0777)) < 0) {
    printf("termina_execprocessod:\n");
    printf("Error creating queue %d!\n", queueKey);
    return 1;
  }

  // Send kill request
  killMsg.kill = 1;
  msgsnd(queueId, &killMsg, sizeof(killMsg), 0);

  return 0;
}
