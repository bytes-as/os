#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>
#include <sys/shm.h>


  // struct mesg_buffer {
  //     long mesg_type;
  //     char number[2];
  // } message;

// status of the process 0 for sleeping and 1 for running
_Bool status = 1;

void handle_signal(int signal_int) {
  if(signal_int == SIGUSR1) {// to make it sleep
    status = 0;
    printf("sigusr1 is received ... \n");
    signal(SIGUSR1, handle_signal);
  }
  if(signal_int == SIGUSR2) {// to make it running
    status = 1;
    signal(SIGUSR2, handle_signal);
  }
}

int main(int argc, char *argv[]) {
  signal(SIGUSR1, handle_signal);
  signal(SIGUSR2, handle_signal);
  printf("%d   ............  ....\n", SIGUSR1);
  printf("strating the process...\n");
  if(argc < 2){
    printf("Missing arguments...\n");
    return -1;
  }
  char *prs;
  prs = argv[1];
  printf("%s\n", prs);
  printf("printing done...");
  // key = ftok(prs, 10);  // page reference string and random 10 to create a unique key
  // msgid = msgget(key, 0666 | IPC_CREAT);
  int i = 0;
  // while(1){
  //   // if(i == strlen(prs)){
  //   //   message.mesg_type = 1;
  //   //   strcpy(message.number,"-9");
  //   //   msgsnd(msgid, &message, sizeof(message), 0);
  //   //   print("message send : %s\n", message.number);
  //   //   break;
  //   // }
  //   // if(!status) {
  //   //   print("sleeping ZZzzz.z.zzz..\n")
  //   //   usleep(100000);
  //   // }
  //   // printf("process is running ... \n");
  //
  //   // msgsnd to send message
  //   // message.mesg_type = 1;
  //   // message.number[0] = prs[i];
  //   // message.number[1] = '\0';
  //   // msgsnd(msgid, &message, sizeof(message), 0);
  //   // print("page number send : %s\n", message.number);
  //   // msgrcv(msgid, &message, sizeof(message), 1, 0);
  //   // print("frame number received : %s", message.number);
  //   // if(!strcpy(message.number, "-2")) {
  //     // printf("Invalid Page reference...\n");
  //     // return 1;
  //   // }
  //   // if(!strcpy(message.number, "-1")) {
  //   //   // putting the whole process to wait
  //   if(status == 0) {
  //     printf("signal received...\n");
  //     break;
  //   }
  //     // continue;
  //   // }
  //   i++;
  // }
  while(1){
    if(status == 0){
      printf("signal received...\n");
      break;
    }
  }
  printf("Terminating process...\n");
  return 0;
}
