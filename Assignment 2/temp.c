#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
 #include <sys/wait.h>

#define MAXWORDS 1024

char *getInput(){
  printf(">>> ");
  char *buffer;
  buffer = (char *)malloc(100 * sizeof(char));
  scanf("%[^\n]%*c", buffer);
  return buffer;
}

_Bool pipeFound(char *s){
  int i=0;
  while(s[i]){
    if(s[i] == '|')
      return 1;
    i++;
  }
  return 0;
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

void pipeParsed(char *buffer, char **pipe_stripped){
  pipe_stripped[0] = strsep(&buffer, "|");
  pipe_stripped[1] = buffer;
}

int wordParsed(char *buffer, char **word_stripped){
  int i=0;
  while(1){
    word_stripped[i] = strsep(&buffer, " ");
    if(!word_stripped[i])
      break;
    i++;
  }
  // while(strsep(&buffer, " ")){
  //   word_stripped[i] = strsep(&buffer, " ");
  //   i++;
  // }
  return i;
}

// int pipeHandler(char *buffer){
//   char **token;
//   token* = (char **)malloc(100 * sizeof(char *));
//   int count;
//   count = wordParsed(buffer, token);
//   int p[2];
//   if(p < 0)
//     exit(1);
//
// }

void stringParsing(char *buffer, char **stripped, char **token){
  _Bool p = pipeFound(buffer);
  _Bool lb = lbFound(buffer);
  _Bool rb = rbFound(buffer);
  _Bool amb = ambFound(buffer);
  if(!p && !lb && !rb && !amb){
    int count;
    count = wordParsed(buffer, token);
    execArgs(token);
  }
  else  {
    if(!p && !lb && !rb){
      int count;
      count = wordParsed(buffer, token);
      token[count-1] = NULL;
      if(fork() == 0){
        printf("closing the standard output\n");
        fclose(stdout);
        printf("stdout check");
        execArgs(token);
      }
    }
  }
}


int main()
{
  char *buffer;
  char **token, *stripped[2];
  int i=0;
  token = (char **)malloc(100 * sizeof(char *));
  for(i=0;i<100; i++)
    token[i] = (char *)malloc(100 * sizeof(char));

  while(1){
    buffer = getInput();
    if(strcmp(buffer, "quit") == 0)
      break;
    stringParsing(buffer, stripped, token);
    wait(NULL);
  }
  return 0;
}
