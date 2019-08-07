#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

// taking input from the STDIN and returnning the input string
char* takeInput(){
  printf(">>>");
  int i;
  char *input;
  input  = (char *)malloc(100 * sizeof(char));
  for(i=0; i<100; i++)
    input[i] = '\0';
  scanf("%[^\n]%*c", input);
  return input;
}

void execArgs(char** args){
  pid_t pid = fork();
  if(pid == -1){
    printf("invoking fork() failed.\n");
    return;
  }
  if(pid == 0){
    if(execvp(args[0], args) < 0)
      perror("couldn't execute command : ");
    else{
      wait(NULL);
      return;
    }
  }
}

void stringParsing(char *buffer){
  char **token;
  token = (char **)malloc(100 * sizeof(char *));
  int i = 0;
  token[i] = strtok(buffer, " ");
  while(token[i]){
    i++;
    token[i] = strtok(NULL, " ");
  }
  printf("The token entered are  : \n");
  int j=0;
  while(j<i){
    printf("%d : %s\n", j, token[j]);
    j++;
  }
}

int main(int argc, char const *argv[]) {
  char* cmd;

  while(1){
    cmd = takeInput();
    if(strcmp(cmd, "quit") == 0)
      exit(EXIT_SUCCESS);
      stringParsing(cmd);
    wait(NULL);
  }
  return 0;
}
