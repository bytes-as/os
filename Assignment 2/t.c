#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

// taking input from the STDIN and returnning the input string
char* takeInput(){
  printf(">>> ");
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

int wordParsed(char *buffer, char **word_stripped){
  int i=0;
  word_stripped[i] = strtok(buffer, " ");
  while(word_stripped[i]){
    i++;
    word_stripped[i] = strtok(NULL, " ");
  }
  // printf("The followling are the words taken from the command: \n");
  // i=0;
  // while(word_stripped[i]){
  //   printf("%d : %s\n", i, word_stripped[i]);
  //   i++;
  // }
  return i;
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

void commandExecutionHandler(char *command){
  char **token;
  int i, count;
  token = (char **)malloc(100 * sizeof(char *));
  for(i=0; i<100; i++)
    token[i] = (char *)malloc(100 * sizeof(char));
  count = wordParsed(command, token);
  if(token[count-1] == "&"){
    token[count-1] = NULL;
    execArgsAmb(token);
  } else
    execArgs(token);
}

// void redirectionCommandExecutionHandler(char *command){
//   _Bool lb , rb
// }

void stringParsingHandler(char **command, int command_count){
  int i;
  _Bool lb, rb, amb;
  lb = lbFound(command[0]);
  rb = rbFound(command[command_count-1]);
  amb = ambFound(command[command_count-1]);
  if(command_count == 1){
    char **token;
    token = (char **)malloc(100 * sizeof(char *));
    for(i=0; i<100; i++)
      token[i] = (char *)malloc(100 * sizeof(char));
    int count;
    count = wordParsed(command[0], token);
    // simple independent command that doesn't need any redirection
    if(!lb && !rb && !amb)
      execArgs(token);
    // if(lb && !rb && !amb){
    //     dup2(0, token[])
    // }

  } else  {
  // creating the pipes corresponding to each command in the string given as input
  // for( i = 0; i < num-pipes; i++ ){
    int i=0;
    int *pipefd;
    pipefd = (int *)malloc(2 * command_count * sizeof(int));
    for(i=0; i<command_count; i++){
      if( pipe(&pipefd[2*i]) < 0 ){
          perror("error creating pipes : ");
          exit(1);
      }
    }
    // int pipefd[1];
    // if(pipe(pipefd) < 0){
    //   perror("error creating pipe :");
    //   exit(1);
    // }

    // first process
    if(fork() == 0){
      // close(*(pipefd));
      // open(*(pipefd+1));
      // dup2(1, *(pipefd+1));
      if(!lb){
        dup2(pipefd[1], 1);
        commandExecutionHandler(command[0]);
      }
      // preocess demands redirection of the input stream
      else{
        // redirecting the input from the argument passed in the command hasn't been implemented
        dup2(pipefd[1], 1);
        // opwn up the input file here then run the execvp command
        // execArgs(token);
      }
    }

    // from the second process til the second last process
    for(i=1; i<command_count-1; i++){
      // child process
      if(fork() == 0){
        dup2(pipefd[2*(i-1)], 0);
        dup2(pipefd[2*i + 1], 1);
        commandExecutionHandler(command[i]);
      }
      wait(NULL);
      // parent process
    }
    // last process to be executed
    if(i == (command_count-1))
      if(fork() == 0){
        // open(pipefd[2*(i-1)]);
        // close(pipefd[2*(i-1) + 1]);
        dup2(pipefd[2*(i-1)], 0);
        if(rb)
          commandExecutionHandler(command[i]);
        else  if(rb)  {
          // int output = open("<file name>", O_WRONLY ,0644);
            //  dup2(output, STDOUT_FILENO);
            //  close (output);
            //
            //
            //
            commandExecutionHandler(command[i]);
        } else {
          commandExecutionHandler(command[i]);
        }
      }
  } // end else
}

void stringParsing(char *buffer){
  int i;
  char **command;
  command = (char **)malloc(100 * sizeof(char *));
  int command_count = pipeParsed(buffer, command);
  if(!command)
    return ;
  stringParsingHandler(command, command_count);
}

int main(int argc, char const *argv[]) {
  char* cmd;

  while(1){
    cmd = takeInput();
    if(strcmp(cmd, "exit") == 0) break;
      // exit(EXIT_SUCCESS);
    else stringParsing(cmd);
    wait(NULL);
  }
  return 0;
}
