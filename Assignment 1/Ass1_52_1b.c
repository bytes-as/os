#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char const *argv[]) {
  /* code */
  int i;
  char cmd[100];
  while(1){
    // taking command input from command line
    scanf(" %[^\n]%*c",cmd);

    // if typed quit, all process will terminate
    if(strcmp(cmd, "quit") == 0)
      break;

    // CLONING THE PROCESS TO RUN THE COMMAND ENTERED BY THE USER
    if(!fork()){    // CHILD PROCESS
      if(execlp(cmd, cmd , NULL) == -1) // Sanitizing the exec() statement for error detection
        perror("exec failed || Check the command");
    }
  }
  return 0;
}
