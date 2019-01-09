#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char const *argv[]) {
  /* code */
  int i;
  char cmd[100];
  while(1){
    scanf(" %[^\n]%*c",cmd);
    // if(argc == 1){
    //   printf("NO ARGUMENTS DETECTED\n");
    // } else  {
          if(strcmp(cmd, "quit") == 0)
            break;
    // }
    // printing the command line arguments
    for(int i=0; i<argc; i++){
      printf("%s\n", argv[i]);
    }

    // CLONING THE PROCESS TO RUN THE COMMAND ENTERED BY THE USER
    if(!fork()){    // CHILD PROCESS
      if(execlp(cmd, cmd , NULL) == -1)
        perror("exec");
    }
  }
  return 0;
}
