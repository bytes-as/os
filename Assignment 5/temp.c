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

int a = 1;

void handle_sigint(int sig)
{
  printf("SIGUSR received... :  %d\n", sig);
  if(sig == SIGUSR1){
    a++;
  }
  sleep(1);
}

int main()
{
    signal(SIGUSR1, handle_sigint);
    while (a == 1) {
      sleep(1);
      printf("still running...\n");
    }
    printf("terminating....\n");
    sleep(1);
    return 0;
}
