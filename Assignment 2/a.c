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

int main()
{
  int i=0, j;
  char *buffer;
  buffer = (char *)malloc(100 * sizeof(char));
  scanf("%[^\n]%*c", buffer);

  char **command;
  command = (char **)malloc(100 * sizeof(char *));

  char ***token;
  token = (char ***)malloc(100 * sizeof(char **));
  for(i=0; i<100; i++){
    token[i] = (char **)malloc(100 * sizeof(char *));
    for(j=0; j<100; j++)
      token[i][j] = (char *)malloc(100 * sizeof(char));
  }
  int command_count = pipeParsed(buffer, command);
  for(i=0;i<command_count;i++){
    wordParsed(command[i], token[i]);
  }
  int pipefd[2];
  if(pipe(pipefd) < 0){
    perror("error creating the pipe : ");
    exit(EXIT_FAILURE);
  }

  printf("The entered command is as follows: \n");
  for(i=0; i<2; i++){
	   j=0;
	  while(token[i][j]){
		  printf("%s	", token[i][j]);
	    j++;
	  }
	  printf("\n");
  }
  for(i=0;i<2;i++){
    if(fork() == 0){
      if(i == 0){
        close(pipefd[0]);
        if(dup2(pipefd[1] , 1) < 0){
          perror("dup1");
          exit(EXIT_FAILURE);
        }
        execArgs(token[0]);
      }
      if(i==1){
        close(pipefd[1]);

        if(dup2(pipefd[0], 0) == -1){
            perror("dup2");
            exit(EXIT_FAILURE);
        }
        execArgs(token[1]);
      }
    } else  {
      wait(NULL);
    }
  }
  return 0;
}
