#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

void execArgs(char** args){
  pid_t pid = fork();
  if(pid == -1){
    printf("invoking fork() failed.\n");
    return;
  }
  if(pid == 0){
    // int i=0;
    // while(args[i]){
    //   printf("%d : %s\n",i, args[i]);
    //   i++;
    // }
    if(execvp(args[0], args) < 0){
      perror("couldn't execute command : ");
      exit(EXIT_FAILURE);
    }
    else{
      wait(NULL);
      return;
    }
  }
}

char* takeInput(){
  int i;
  char *user, *host, *input;
  host = (char *)malloc(100 * sizeof(char));
  user = (char *)malloc(100 * sizeof(char));
  user = getlogin();
  int h = gethostname(host, 100);
  printf("%s@%s: ", user, host);
  input  = (char *)malloc(100 * sizeof(char));
  for(i=0; i<100; i++)
    input[i] = '\0';
  scanf("%[^\n]%*c", input);
  return input;
}

int pipeParsed(char *buffer, char **pipe_stripped){
  int i=0;
  pipe_stripped[i] = strtok(buffer, "|");
  while(pipe_stripped[i]){
    i++;
    pipe_stripped[i] = strtok(NULL, "|");
  }
  return i;
}

int spaceParsed(char *buffer, char **space_stripped){
  int i=0;
  space_stripped[i] = strtok(buffer, " ");
  while(space_stripped[i]){
    i++;
    space_stripped[i] = strtok(NULL, " ");
  }
  return i;
}

void commandExecution(char **command, char *source, char *destination, char *error){
  // call execArgs to finally execute the command
  if(!source){
    int input = open(source, O_RDONLY);
    dup2(input, STDIN_FILENO);
    close(input);
  }

  if(!destination){
    int output = open(destination, O_WRONLY|O_CREAT|O_TRUNC, 0666);
    dup2(output, STDOUT_FILENO);
    close(output);
  }

  if(!error){
    int err = open(error, O_WRONLY|O_CREAT|O_TRUNC, 0666);
    dup2(err, STDERR_FILENO);
    close(err);
  }

  execArgs(command);
}

// shoud return the values of the source and destination and error don't know what the fuck is that
void commandExecutionHandler(char **token, char **component, int token_count){
  
}

void stringParsing(char *buffer){
  int i, j, command_count, *token_count;
  char **command, ***token;
  command = (char **)malloc(100 * sizeof(char *));
  for(i=0; i<100; i++)
    command[i] = (char *)malloc(100 * sizeof(char));
  command_count = pipeParsed(buffer, command);
  token_count = (int *)malloc(command_count * sizeof(int));
  // printf("Number of commands in the string : %d\n", command_count);
  token = (char ***)malloc(command_count * sizeof(char **));
  for(i=0; i<command_count; i++){
    token[i] = (char **)malloc(100 * sizeof(char *));
    for(j=0; j<100; j++)
      token[i][j] = (char *)malloc(100 * sizeof(char));
    token_count[i] = spaceParsed(command[i], token[i]);
    // printf("Number of token in command %d : %d\n",i, token_count[i]);
  }
  // testing the tokens by printing
  // for(i=0; i<command_count; i++){
  //   j=0;
  //   while(token[i][j]){
  //     printf("%s ", token[i][j]);
  //     j++;
  //   }
  //   printf("\n");
  // }
}

int main(int argc, char const *argv[]) {
  char *buffer;
  while(1){
    buffer = takeInput();
    if(!buffer || !strcmp(buffer, "") || !strcmp(buffer, "exit"))
      break;
    stringParsing(buffer);
  }
  return 0;
}
