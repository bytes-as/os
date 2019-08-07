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

void execArgsAmb(char** args){
  pid_t pid = fork();
  if(pid == -1){
    printf("invoking fork() failed.\n");
    return;
  }
  if(pid == 0){
    if(execvp(args[0], args) < 0)
      perror("couldn't execute command : ");
    else
      return;
  }
}

int pipeFound(char *s){
  int i=0;
  int count = 0;
  while(s[i]){
    if(s[i] == '|')
      count++;
    i++;
  }
  return count;
}

_Bool lbFound(char *s){
  int i=0;
  while(s[i]){
    if(s[i] == '<')
      return 1;
    i++;
  }
  return 0;
}

_Bool rbFound(char *s){
  int i=0;
  while(s[i]){
    if(s[i] == '>')
      return 1;
    i++;
  }
  return 0;
}

_Bool ambFound(char *s){
  int i=0;
  for(i=0;i<strlen(s); i++)
    if(s[i] == '&')
      return 1;
  return 0;
}

int wordParsed(char *buffer, char **word_stripped){
  int i=0;
  while(1){
    word_stripped[i] = strsep(&buffer, " ");
    if(!word_stripped[i])
      break;
    i++;
  }
  return i;
}

int pipeParsed(char *buffer, char **pipe_stripped){
  int i=0;
  while(1){
    pipe_stripped[i] = strsep(&buffer, "|");
    if(!pipe_stripped[i])
      break;
    i++;
  }
  // pipe_stripped[0] = strsep(&buffer, "|");
  // pipe_stripped[1] = buffer;
  return i;
}

int stringProcessingHandler(char *cmd, _Bool forward, _Bool first_flag, int *pipefd){
  int i, count;

  char **token, *stripped[2];
  token = (char **)malloc(100 * sizeof(char));
  for(i=0; i<100; i++)
    token[i] = (char *)malloc(100 * sizeof(char));

  _Bool rb, lb, amb;
  rb = rbFound(cmd);
  lb = lbFound(cmd);
  amb = ambFound(cmd);
  // simple command without any special characters
  if(!rb && !lb && !amb && !forward){
    count = wordParsed(cmd, token);
    // printf("the number of token is : %d\n", count);
    execArgs(token);
    return 0;
  } else {
    // command without any special characters ending with '&'
    if(!rb && !lb && !forward){
    count = wordParsed(cmd, token);
    token[count-1] = NULL;
    execArgsAmb(token);
    return 0;
    } else  {
      count = wordParsed(cmd, token);
      if(first_flag && forward){
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);
        execArgs(token);
      } else  {
        if(!first_flag && forward){
          count = wordParsed(cmd, token);
          dup2(pipefd[0], STDIN_FILENO);
          dup2(pipefd[1], STDOUT_FILENO);
          execArgs(token);
        } else  {
          if(!first_flag && !forward){
            close(pipefd[1]);
            dup2(pipefd[0], STDIN_FILENO);
            count = wordParsed(cmd, token);
            execArgs(token);
          }
        }
      }
    }
  }
}

void srtingParcing(char *cmd){
  int i;
  int count;
  char **pipe_parsed;
  pipe_parsed = (char **)malloc(100 * sizeof(char *));
  for(i=0; i<100; i++)
    pipe_parsed[i] = (char *)malloc(100 * sizeof(char));
  // count will be the number of commands that has to be executed
  count = pipeParsed(cmd, pipe_parsed);
  
}

int main(int argc, char const *argv[]) {
  char* cmd;

  while(1){
    cmd = takeInput();
    if(strcmp(cmd, "quit") == 0)
      exit(EXIT_SUCCESS);
      srtingParcing(cmd);
    wait(NULL);
  }
  return 0;
}
