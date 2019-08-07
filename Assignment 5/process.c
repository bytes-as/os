#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>
#include <sys/shm.h>


struct mesg_buffer {
    long mesg_type;
    char number[2];
} message;

// status of the process 0 for sleeping and 1 for running
_Bool status = 0;

void signal_handler(int signal_int) {
  if(signal_int == SIGUSR1) // to make it sleep
    status = 0;
  if(signal_int == SIGUSR2) // to make it running
    status = 1;
  else
    status = 0;
  signal(SIGUSR1, signal_handler);
  signal(SIGUSR2, signal_handler);
}

int main(int argc, char *argv[]) {
  // prs, MQ1, MQ3
  if(argc < 4){
    printf("Missing arguments...\n");
    return -1;
  }
  signal(SIGUSR1, signal_handler);
  signal(SIGUSR2, signal_handler);
  char *prs;
  prs = argv[1];
  printf("%s", prs);
  // key_t key = ftok(prs, 10);  // page reference string and random 10 to create a unique key
  // int msgid = msgget(key, 0666 | IPC_CREAT);
  int MQ1, MQ2;
  // memcpy(&MQ1, &argv[2], strlen(argv[2]));
  // memcpy(&MQ3, &argv[3], strlen(argv[3]));
  MQ1 = atoi(argv[2]);
  MQ3 = atoi(argv[3]);
  int i = 0;
  while(1){
    if(i == strlen(prs)){
      message.mesg_type = 1;
      memset(message.number, '\0', 2);
      strcpy(message.number,"-9");
      msgsnd(MQ3, &message, sizeof(message), 0);
      printf("message send : %s\n", message.number);
      break;
    }
    if(!status) {
      printf("sleeping ZZzzz.z.zzz..\n");
      usleep(100000);
      continue;
    }
    printf("process is running ... \n");

    // msgsnd to send message
    message.mesg_type = 1;
    memset(message.number, '\0', 2*sizeof(char));
    memcpy(message.number, &prs[i], sizeof(char));
    // message.number[0] = prs[i];
    // message.number[1] = '\0';
    msgsnd(MQ3, &message, sizeof(message), 0);
    printf("page number send : %s\n", message.number);
    memset(message.number, '\0', 2);
    msgrcv(MQ3, &message, sizeof(message), 1, 0);
    printf("frame number received : %s", message.number);
    if(!strcpy(message.number, "-2")) {
      printf("Invalid Page reference...\n");
      return 1;
    }
    if(!strcpy(message.number, "-1")) {
      // putting the whole process to wait
      status = 0;
      continue;
    }
    i++;
  }
  printf("Terminating process...\n");
  return 0;
}
