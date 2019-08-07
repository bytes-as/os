#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>
#include <sys/shm.h>
#include <sys/wait.h>

int main(int argc, char const *argv[]) {
  int i=0, k;
  printf("k : ");
  scanf("%d", &k);
  pid_t process_id[k];
  // _Bool status[k];
  // memset(status, 1, k);
  while (i < k) {
    usleep(250000);
    if((process_id[i] = fork()) == 0) {
      printf("child process : %d running ....\n", i);
      execlp("xterm","xterm", "-hold", "-e","./hell",(char *)(NULL));
    }
    i++;
  }
  i=0;
  while (i<k) {
    usleep(500000);
    printf("parent_process loop %d, sending signal %d\n", i, SIGUSR1);
    kill(process_id[i], SIGUSR1);
    i++;
  }
  return 0;
}
