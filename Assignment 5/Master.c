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


// reference range would be b/w 0 and 9
#define REFERENCE_RANGE 9

// struct free_frame {
//   int frame_number;
//   struct free_frame_list* next;
// } ;

struct _page_table {
  int page;
  int frame;
} page_table;

int main(int argc, char const *argv[]) {
  // create and initialize all the data structure
  // run the module sched.c with the exec command
  // run the module MMU.c with the exec command
  //    for command line arguments would be:
  //    1. page table(SM1) -->TBD
  //    2. free frame list(SM2) --> TBD
  //    3. MQ1 --> TBD
  //    3. MQ2 --> done
  //    4. MQ3 --> done
  srand(time(NULL));
  int k, m, f, *prs_size;
  float random_float;
  key_t key1 = ftok("MQ1", 10);
  int MQ1 = msgget(key1, 0666 | IPC_CREAT);
  char MQ1_string[20];
  sprintf(MQ1_string, "%d", MQ1);
  key_t key2 = ftok("MQ2", 10);
  int MQ2 = msgget(key2, 0666 | IPC_CREAT);
  char MQ2_string[20];
  sprintf(MQ2_string, "%d", MQ2);
  key_t key3 = ftok("MQ3", 10);
  int MQ3 = msgget(key3, 0666 | IPC_CREAT);
  char MQ3_string[20];
  sprintf(MQ3_string, "%d", MQ3);
  key_t key4 = ftok("SM1",10);
  int SM1 = shmget(key4,1024,0666|IPC_CREAT);
  char SM1_string[20];
  sprintf(SM1_string, "%d", SM1);
  key_t key5 = ftok("SM2",10);
  int SM2 = shmget(key5,f*sizeof(int),0666|IPC_CREAT);
  char SM2_string[20];
  sprintf(SM2_string, "%d", SM2);
  // attaching shared memory of size of the linked list containf the free frame number
  // shmget returns an identifier in shmid
  // convert these integers to string to send as command line arguments
  printf("Enter the number of processes to be executed(k) : ");
  scanf("%d", &k);
  prs_size = (int *)malloc(k * sizeof(int));
  printf("Enter the maximum length of the page reference string(m) : 10 * ");
  scanf("%d", &m);
  printf("Enter the number of frames : ");
  scanf("%d", &f);
  int i = 0, j;
  char **prs;
  pid_t process_id[k];
  pid_t scheduler_id;
  pid_t mmu_id;
  prs = (char **)malloc(sizeof(char *));
  // scheduler_id = fork();
  // if(scheduler_id == 0) {
  //   // send MQ1, MQ2 as arguments
  //   execlp("gcc", "gcc", "sched.c", "-o", "sched", NULL);
  //   execlp("xterm","xterm", "-hold", "-e","./sched",prs[i], MQ1_string, MQ2_string, (char *)(NULL));
  // }
  // mmu_id = fork();
  // if(mmu_id == 0) {
  //   // send MQ2, MQ3, SM1, SM2
  //   execlp("gcc", "gcc", "MMU.c", "-o", "mmu", NULL);
  //   execlp("xterm","xterm", "-hold", "-e","./mmu",MQ2_string, MQ3_string, SM1_string, SM2_string,(char *)(NULL));
  // }
  while(i < k) {
    random_float = 1 + ((float)rand() / RAND_MAX) * (m-1);
    prs_size[i] = 2*random_float + (float)(8 * random_float) * ((float)rand()/ RAND_MAX);
    // printf("%d\t%d\n", prs_size[i], i);
    // create a table fo reach process;
    struct _page_table pt[prs_size[i]];
    j = 0;
    prs[i] = (char *)malloc((2 * prs_size[i] * sizeof(char)) - 1);
    while(j < prs_size[i]) {
      prs[i][2*j] = '0' + rand() % REFERENCE_RANGE;
      if(j != prs_size[i]-1)
        prs[i][2*j+1] = ',';
      j += 1;
    }
    printf("Process %d || %s :: %d\n",i, prs[i], prs_size[i]);
    // run the module process.c with the exec command
    process_id[i] = fork();
    if(process_id[i] == 0) {
      // send prs , MQ1, MQ3
      // execlp("gcc", "gcc", "_process.c", "-o", "process", NULL);
      // wait(NULL);
      printf("turning on the emulator\n");
      execlp("xterm","xterm", "-hold", "-e","./process",prs[i], MQ1_string, MQ3_string,(char *)(NULL));
    }
    // execlp("process.c", "process.c", &prs[i], NULL);
    usleep(250000);
    i++;
  }

  i=-1;
  while(i < 10) {
    i++;
    kill(process_id[i], SIGINT);
    usleep(500000);
  }

  // FREE FRAME LIST - contains a list of free frames in main memory
  //    used by MMU
  //    implemented by shared memory SM2
  // Ready Queue - used by the scheduler
  //     implemented by the message queue MQ1
  // communication b/w scheduler and MMU - MQ2
  //                   process-es and MMU - MQ3

  return 0;
}
