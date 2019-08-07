#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

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

void stringParsing(char * buffer){

}

int main()
{
  char *buffer;
  while(1){
    buffer = takeInput();
    if(strcmp(buffer, "exit")==0)
      break;
    if(!buffer)
      continue;
    stringParsing(buffer);
  }
  return 0;
}
