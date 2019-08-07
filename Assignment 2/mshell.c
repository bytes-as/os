#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<string.h>
#include <fcntl.h>
#define MAXLIST 100
#define NMAX 1024

char* strip(char *str) {
    if(str==NULL)return NULL;
    int count = 0;
    for (int i = 0; str[i]; i++)
        if (str[i] != ' ' && str[i]!='\n')
                str[count++] = str[i];
    str[count] = '\0';
    return str;
}

int split_space(char* str, char** str_arr){
    for (int i = 0; i < MAXLIST; i++) {
        str_arr[i] = strip(strsep(&str, " "));
        if (str_arr[i] == NULL)
                return i;
        if (strlen(str_arr[i]) == 0) i--;
    }
    return MAXLIST;
}

int fetch_in_out_file(char *cmd, char **splits){
    splits[0] = splits[1] = splits[2] = splits[3] = NULL;

    char *err = strstr(cmd,"2>");
    char *in = strchr(cmd,'<');
    char *out = strchr(cmd,'>');
    if(out != NULL){
        if(err != NULL && out == err+1){
            out = strchr(err+2,'>');
        }
        if(out != NULL){
            *out = '\0';
            splits[2] = out + 1;
        }
    }
    if(err != NULL){
        *err = '\0';
        *(err+1) = '\0';
        splits[3] = err + 2;
        // printf("err%s\n",splits[3] );
    }
    if(in != NULL ){
        *in = '\0';
        splits[1] = in+1;
    }
    splits[0] = cmd;
    for(int i = 1;i<4;i++) splits[i] = strip(splits[i]);
    // for(int i = 1;i<4;i++)printf("%s\n", splits[i]);
    return 0;
}

int execute(char **cmd_arr, char *input_file, char *output_file, char *err_file){
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
    if(err_file != NULL){
        int err = open(err_file,O_WRONLY|O_CREAT|O_TRUNC,0666);
        dup2(err,STDERR_FILENO);
        close(err);
    }
    if (execvp(cmd_arr[0], cmd_arr) == -1){
        perror("execute failed !!\n");
        _exit(EXIT_FAILURE);
    }
}



int run(char *cmd, int input, bool is_first_cmd, bool is_last_cmd, bool bg = false){
    char *cmd_arr[MAXLIST], *splits[4];

    int f = fetch_in_out_file(cmd, splits);
    int n = split_space(splits[0],cmd_arr);

    int pipeeee[2];
    pipe( pipeeee );

    // for(int i = 0;i<n;i++)printf("%s ",cmd_arr[i] );

    // printf(" f: %d l:%d i:%d p0:%d p1:%d\n",
        // is_first_cmd,is_last_cmd,input,pipeeee[0],pipeeee[1]);
    int pid = fork();

    if (pid == 0) {
        if (is_first_cmd && ! is_last_cmd && input == 0) {
            // printf("f\n");
            dup2( pipeeee[1], STDOUT_FILENO );
        }
        else if (!is_first_cmd && !is_last_cmd  && input != 0) {
            // printf("m\n");
            dup2(input, STDIN_FILENO);
            dup2(pipeeee[1], STDOUT_FILENO);
        }
        else  {
            // printf("l\n");
            dup2( input, STDIN_FILENO );
        }
        execute(cmd_arr, splits[1] ,splits[2], splits[3]);
        exit(pipeeee[0]);
    }
    else{
        if(!(bg && is_last_cmd))
            wait(NULL);
    }
    if (input != 0)
        close(input);
    close(pipeeee[1]);
    if (is_last_cmd)
        close(pipeeee[0]);

    return pipeeee[0];
}

int main()
{
    char line[NMAX];
    char* username = getenv("USER"); 
    while (1) {
        printf("(%s) $: ",username);
        if (!fgets(line, NMAX, stdin))
            return 0;

        if(strcmp(line,"exit\n")==0)
            exit(0);


        int input = 0;
        char *cmd = line;
        bool is_first_cmd = true,bg = false;
        char *next = strchr(cmd, '|');

        while (next != NULL) {
            *next = '\0';
            input = run(cmd, input, is_first_cmd, false);

            cmd = next + 1;
            is_first_cmd = false;
            next = strchr(cmd, '|');
        }
        char *amp = strchr(cmd, '&');
        if(amp!=NULL){
            bg = true;
            *amp='\0';
        }
        input = run(cmd, input, is_first_cmd, true,bg);
        // bg=false;
        // if(bg)wait(NULL);
    }
    return 0;
}
