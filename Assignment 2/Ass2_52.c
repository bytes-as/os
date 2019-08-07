#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#define MAXLEN 100
#define MAXCMD 100
#define MAXBUF 1024

// taking input from the STDIN and returnning the input string
char* takeInput(){
  int i;
  char *user, *host, *input;
  host = (char *)malloc(100 * sizeof(char));
  user = (char *)malloc(100 * sizeof(char));
  user = getlogin();
  int h = gethostname(host, 100);
  printf("%s@%s: ", user, host);
  input  = (char *)malloc(MAXBUF * sizeof(char));
  for(i=0; i<100; i++)  input[i] = '\0';
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

void commandExecutionHandler(char **cmd_arr, char *input_file, char *output_file){
    if(input_file != NULL){
        int in = open(input_file,O_RDONLY);
        dup2(in,STDIN_FILENO);
        close(in);
    }
    if(output_file != NULL){
        int out = open(output_file,O_WRONLY|O_CREAT|O_TRUNC,0666);
        dup2(out,STDOUT_FILENO);
        close(out);
    }
    execArgs(cmd_arr);
    return ;
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
  // printf("wordParsed() output : \n");
  // int j;
  // for(j=0; j<i; j++)
  //   printf("%d : %s\n",j ,word_stripped[j]);
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

void split(char *command, char **part){
  char *source = strstr(command, "<");
  char *destination = strstr(command, ">");
  part[0] = part[1] = part[2] = NULL;
  _Bool amb = 0;
  amb = ambFound(command);
  int i=0;
  if(source){
    *source = '\0';
    part[1] = source + 2;
  }
  if(destination){
    *destination = '\0';
    part[2] = destination + 2;
  }
  part[0] = command;
}

int stringParserUtility(char **command){
  int i, j;
  char ***token;
  token = (char ***)malloc(3 * sizeof(char **));
  token[0] = (char **)malloc(100 * sizeof(char *));
  for(i=0; i<100; i++)  token[0][i] = (char *)malloc(100 * sizeof(char));
  for(i=1; i<3; i++){
    token[i] = (char **)malloc(1 * sizeof(char *));
    for(j=0; j< 100; j++)
      token[i][j] = (char *)malloc(100 * sizeof(char));
  }
  wordParsed(command[0], token[0]);
  wordParsed(command[1], token[1]);
  wordParsed(command[2], token[2]);
  // printf("tokens are : \n");
  // for(i=0;i<3;i++){
  //   printf("%d ::\n", i);
  //   j=0;
  //   while(token[i][j]){
  //     printf("%s\n", token[i][j]);
  //     j++;
  //   }
  // }
  commandExecutionHandler(token[0], token[1][0], token[2][0]);
  return 0;
}

void stringParser(char *buffer){
  int i, j, command_count, pipefd[2], pipefd_status;
  pipe(pipefd);
  char **command;
  command = (char **)malloc(MAXCMD * sizeof(char));
  for(i=0; i<MAXCMD; i++)
    command[i] = (char *)malloc(MAXLEN * sizeof(char));
  command_count = pipeParsed(buffer, command);
  char **part;
  part = (char **)malloc(3 * sizeof(char *));
  for(i=0; i<3; i++)
    part[i] = (char *)malloc(100 * sizeof(char));
  _Bool first_flag = 1;

  for(i=0; i<command_count; i++){
    for(j=0; j<3; j++)
      part[i] = NULL;
    split(command[i], part);
    // if(command_count == 1){
    //   stringParserUtility(part);
    //   // commandExecutionHandler(token[0], token[1][0], token[2][0]);
    // } else  {
    //   if(pipefd_status < 0){
    //     perror("error creating pipe");
    //     return ;
    //   }
    //   // first process
    //   if(fork() == 0){
    //     // replace the standard output with the output of the pipes
    //     dup2(pipefd[1], 1);
    //     close(pipefd[0]);
    //     stringParserUtility(part);
    //     // commandExecutionHandler(token[0], token[1][0], token[2][0]);
    //   }
    //   // else  {
    //   //   wait(NULL);
    //     // dup2(pipefd[0], 0);
    //     // close(pipefd[1]);
    //     // execArgs(command1);
    //   // }
    //   // wait(NULL);
    //   for(i=1; i<command_count-1; i++){
    //     if(fork() == 0){
    //       // replace the standard output with the output of the pipes
    //       dup2(pipefd[1], 1);
    //       close(pipefd[0]);
    //       stringParserUtility(part);
    //       // commandExecutionHandler(token[0], token[1][0], token[2][0]);
    //     } else  {
    //       wait(NULL);
    //       dup2(pipefd[0], 0);
    //       close(pipefd[1]);
    //       stringParserUtility(part);
    //       // commandExecutionHandler(token[0], token[1][0], token[2][0]);
    //     }
    //     wait(NULL);
    //
    //     // last process
    //     if(fork() == 0){
    //         dup2(pipefd[0], 0);
    //         close(pipefd[0]);
    //         commandExecutionHandler(token[0], token[1][0], token[2][0]);
    //     }
    //   }
    // }
    if(command_count == 1){
      stringParserUtility(part);
      break;
    }
    if(first_flag){
      if(fork() == 0){
        printf("first command module is running.\n");
        dup2(pipefd[1], 1);
        close(pipefd[0]);
        stringParserUtility(part);
      }
      // wait(NULL);
    }
    else if(i == command_count-1)
    {
      if(fork() == 0){
        dup2(pipefd[0], 0);
        close(pipefd[1]);
        stringParserUtility(part);
      }
    } else  {
      if(fork() == 0){
        // replace the standard output with the output of the pipes
        dup2(pipefd[1], 1);
        close(pipefd[0]);
        stringParserUtility(part);
      } else  {
        wait(NULL);
        dup2(pipefd[0], 0);
        close(pipefd[1]);
        stringParserUtility(part);
      }
    }
    first_flag = 0;
    // stringParserUtility(command[i]);
  }
  // printf("all all complete and complete\n");
  return ;
}

int main(int argc, char const *argv[]) {
  char* cmd;

  while(1){
    cmd = takeInput();
    if(strcmp(cmd, "quit") == 0)
      exit(EXIT_SUCCESS);
    stringParser(cmd);
    wait(NULL);
    // break;
  }
  return 0;
}


/*
  if(fork() == 0){
    // replace the standard output with the output of the pipes
    dup2(pipefd[1], 1);
    close(pipefd[0]);
    execArgs(command);
  } else  {
    wait(NULL);
    dup2(pipefd[0], 0);
    close(pipefd[1]);
    execArgs(command1);
  }
  wait(NULL);

*/
