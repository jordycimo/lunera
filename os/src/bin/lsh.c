#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <libgen.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>

#define MAX_ARGS 32
#define MAX_INPUT_LENGTH 1024
#define MAX_JOBS 100
#define MAX_PIPES 500

void banner() {
    printf("\033[1;1H\033[2J");
    printf("LuneraSHell\n");
    printf("builtin commands:\n");
    printf("\'cd\' : change directory\n");
    printf("\'exit\' : terminates shell\n ^!^will cause kernel panic if running as init process^!^\n");
    printf("\'jobs\' : print all currently running jobs\n");
    printf("\'fg\' : bring running job to foreground ??\n");
    printf("\'clear\' : clear screen\n");
    printf("\'help\' : print this help text\n");

}

char currentProcess[MAX_INPUT_LENGTH];
int NumberOfJobs = 0;

struct job
{
    pid_t pid;
    char command[MAX_INPUT_LENGTH];
};

struct job jobs[MAX_JOBS];

// milestone 10 jobs
void add_job(pid_t pid)
{
    jobs[NumberOfJobs].pid = pid;
    strcpy(jobs[NumberOfJobs].command, currentProcess);
    NumberOfJobs++;
}

void print_jobs()
{
    for (int i = 0; i < NumberOfJobs; i++)
    {
        printf("[%d] %s\n", i + 1, jobs[i].command);
    }
}

void remove_job(int job_index)
{
    if(NumberOfJobs==0){
        return;
    }
    for (int i = job_index; i < NumberOfJobs - 1; i++)
    {
        jobs[i].pid = jobs[i + 1].pid;
        strcpy(jobs[i].command, jobs[i + 1].command);
    }
    NumberOfJobs--;
}

//execute_built_in_function 
// 1: continue, 0: break, 2: nothing

int execute_built_in_function(char* args[], int numOfArgs){
    // cd
    if (strcmp(args[0], "cd") == 0)
    {
        if (numOfArgs == 1 || numOfArgs > 2)
        {
            if (chdir("/") != 0)
            {
                fprintf(stderr, "Error: can't change to root directory\n");
            }
        }
        else
        {
            if (chdir(args[1]) != 0)
            {
                fprintf(stderr, "Error: invalid directory\n");
            }
        }
        return 1;
    }

    // exit
    else if (strcmp(args[0], "exit") == 0)
    {
        if (numOfArgs > 1)
        {
            fprintf(stderr, "Error: invalid command\n");
            return 1;
        }
        else
        {
            if (NumberOfJobs!=0){
                fprintf(stderr, "Error: there are suspended jobs\n");
                return 1;
            }
            return 0;
        }
    }

    //jobs
    else if(strcmp(args[0], "jobs") == 0)
    {
        if (numOfArgs > 1)
        {
            fprintf(stderr, "Error: invalid command\n");
            return 1;
        }
        else
        {
            print_jobs();
            return 1;
        }
    }

    //fg
    else if(strcmp(args[0], "fg") == 0)
    {
        if (numOfArgs == 1 || numOfArgs > 2 )
        {
            fprintf(stderr, "Error: invalid command\n");
        }
        else
        {
            int job_idx = atoi(args[1])-1;
            if(job_idx>=NumberOfJobs){
                fprintf(stderr, "Error: invalid job\n");
                return 1;
            }

            pid_t process_id=jobs[job_idx].pid;
            strcpy(currentProcess,jobs[job_idx].command);

            remove_job(job_idx);
            kill(process_id, SIGCONT);

            // Parent process
            int status;
            waitpid(process_id, &status, WUNTRACED);
            if(WIFSTOPPED(status)){
                add_job(process_id);
            }
        }
        return 1;
    }

    //clear
    else if(strcmp(args[0], "clear") == 0)
    {
        printf("\033[1;1H\033[2J");
    }

    //help
    else if(strcmp(args[0], "help") == 0)
    {
        printf("\033[1;1H\033[2J");
        banner();
    }

    return 2;
}

// milestone 6,7
// 0 for continue
int input_output_redirection(char* args[],int* input_fd, int* output_fd){
    int file_not_exist=0;
    for (size_t j = 0; args[j] != NULL; j++)
    {
        if (strcmp(args[j], ">") == 0 || strcmp(args[j], ">>") == 0)
        {
            mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

            if (strcmp(args[j], ">") == 0)
            {
                *output_fd = open(args[j + 1], O_WRONLY | O_CREAT | O_TRUNC, mode);
            }
            else
            {
                *output_fd = open(args[j + 1], O_WRONLY | O_CREAT | O_APPEND, mode);
            }
            if (*output_fd == -1)
            {
                perror("open file failed");
                exit(1);
            }
            args[j] = NULL;
        }
        else if (strcmp(args[j], "<") == 0)
        {
            if(args[j+1]==NULL){
                fprintf(stderr, "Error: invalid command\n");
                file_not_exist=-1;
                break;
            }

            file_not_exist = access(args[j + 1], F_OK);
            if(file_not_exist==-1){
                fprintf(stderr, "Error: invalid file\n");
                break;
            }

            *input_fd = open(args[j + 1], O_RDONLY);
            if (*input_fd == -1)
            {
                fprintf(stderr, "Error: invalid command\n");
                continue;
            }
            args[j] = NULL;
        }
    }
    if(file_not_exist==-1){
        return 0;
    }
    return 1;
}



// execute commands that are not built-in function
int execute_other_command(char* args[], int input_fd, int output_fd){
    // check if user specifies only the base name without any slash(for any other system call)
    char path[MAX_INPUT_LENGTH];
    if (strchr(args[0], '/') == NULL)
    {
        strcpy(path,"/bin/");
        strcat(path, args[0]);
    }
    else{
        strcpy(path,args[0]);
    }
    // Ignore signals
    signal(SIGINT, SIG_IGN);
    signal(SIGQUIT, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);

    // Fork a new process to run the program
    pid_t pid = fork();
    if (pid == -1)
    {
        perror("fork() error");
        exit(1);
    }
    // Child process
    else if (pid == 0)
    {
        signal(SIGINT, SIG_DFL);
        signal(SIGQUIT, SIG_DFL);
        signal(SIGTSTP, SIG_DFL);

        // if need redirect output
        if (output_fd != 1)
        {
            if (dup2(output_fd, STDOUT_FILENO) == -1)
            {
                perror("dup2() error");
            }
        }

        // if need redirect input
        if (input_fd != 0)
        {
            if (dup2(input_fd, STDIN_FILENO) == -1)
            {
                perror("dup2() error");
            }
        }

        if (execvp(path, args) == -1)

        {

            fprintf(stderr, "Error: invalid program\n");

            exit(1);

        };

        

    }

    else

    {

        // Parent process

        int status;

        waitpid(-1, &status, WUNTRACED);

        if(WIFSTOPPED(status)){

            add_job(pid);

        }

    }

}





int execute_command(char* line){

    char *args[MAX_ARGS];

   

    int numOfArgs = 0;

    args[numOfArgs] = strtok(line, " ");

    while (args[numOfArgs] != NULL)

    {

        numOfArgs++;

        args[numOfArgs] = strtok(NULL, " ");

    }



    // Check if the input command is empty

    if (args[0] == NULL)

    {

        exit(1);

    }



    char path[MAX_INPUT_LENGTH];

    if (strchr(args[0], '/') == NULL)

    {

        strcpy(path,"/bin/");

        strcat(path, args[0]);

    }

    else{

        strcpy(path,args[0]);

    }



    // add redirection

    int input_fd=0;

    int output_fd=1;

    input_output_redirection(args,&input_fd,&output_fd);



    pid_t pid = fork();

    if (pid == -1)

    {

        perror("fork() error");

        exit(1);

    }



    // Child process

    else if (pid == 0)

    {

        

        // if need redirect output

        if (output_fd != 1)

        {

            if (dup2(output_fd, STDOUT_FILENO) == -1)

            {

                perror("dup2() error");

            }

        }

        // if need redirect input

        if (input_fd != 0)

        {

            if (dup2(input_fd, STDIN_FILENO) == -1)

            {

                perror("dup2() error");

            }

        }



        if (execvp(path, args) == -1)

        {

            fprintf(stderr, "Error: invalid program\n");

        };

        exit(1);

    }

    else

    {

        // Parent process

        int status;

        waitpid(-1, &status, WUNTRACED);

        if(WIFSTOPPED(status)){

            add_job(pid);

        }

    }



}



int main(int argc, const char *const *argv)
{
    char cwd[MAX_INPUT_LENGTH];
    char *line = NULL;
    char line2[MAX_INPUT_LENGTH];
    size_t len = 0;
    ssize_t nread;
    char *dir;

    banner();
    printf("\033[?12h");


    while (1)
    {
        // milestone 1
        getcwd(cwd, sizeof(cwd));
        dir = basename(cwd);
        printf("%s > ", dir);
        fflush(stdout);

        // milestone 2
        int nread = getline(&line, &len, stdin);
        if (nread == -1)
        {
            break;
        }

        // replace newline with \0
        if (line[nread - 1] == '\n')
        {
            line[nread - 1] = '\0';
        }
        strcpy(line2,line);

        // milestone 10
        //store current command
        strcpy(currentProcess,line);

        // split argumetns
        char *args[MAX_ARGS];

        int numOfArgs = 0;
        args[numOfArgs] = strtok(line, " ");
        while (args[numOfArgs] != NULL)
        {
            numOfArgs++;
            args[numOfArgs] = strtok(NULL, " ");
        }

        // Check if the input command is empty
        if (args[0] == NULL)
        {
            continue;
        }

        // check if it's a built-in command. If so, it should not be executed like other programs.
        int state = execute_built_in_function(args, numOfArgs);
        if(state==1){
            continue;
        }
        else if(state==0){
            break;
        }

        //  input,output redirection
        int input_fd = 0, output_fd = 1;
        if(input_output_redirection(args,&input_fd,&output_fd)==0){
            continue;
        }

        // milestone 7,8 pipe
        // Initialize pipe
        int pipe_fds[MAX_PIPES][2];
        char* args_pipe[500];
        int num_command=0;
        args_pipe[num_command] = strtok(line2, "|");

        while (args_pipe[num_command] != NULL)
        {
            num_command++;
            args_pipe[num_command] = strtok(NULL, "|");
        }

        // Create pipes
        for (int i = 0; i<num_command-1; i++) {
            // Create a pipe
            if (pipe(pipe_fds[i]) == -1) {
                perror("pipe failed");
                exit(1);
            }
        }

        if(strcmp(args[0],"|")==0){
            fprintf(stderr,"Error: invalid command\n");
            continue;
        }

        if (num_command>1)
        {
            // file descriptor for input/output
            int fd_in=0;
            int fd_out=1;

            //we need to create num_command children process
            for (size_t i = 0; i < num_command ; i++)
            {
                pid_t pid = fork();
                if (pid == 0) {
                    //Child process
                    if (i == 0) {
                        dup2(pipe_fds[i][1], 1);
                    } else if (i == num_command-1) {
                        dup2(pipe_fds[i-1][0], 0);
                    } else {            
                        dup2(pipe_fds[i-1][0], 0);
                        dup2(pipe_fds[i][1], 1);
                    }
                    for (int j = 0; j < num_command-1; j++) {
                        close(pipe_fds[j][0]);
                        close(pipe_fds[j][1]);
                    }
                    execute_command(args_pipe[i]);
                    exit(0);
                }
            }

            // parent process
            for (size_t i = 0; i < num_command-1; i++)
            {
                close(pipe_fds[i][0]);
                close(pipe_fds[i][1]);
            }

            for (size_t i = 0; i <num_command; i++)
            {
                wait(NULL);
            }
            continue;
        }
        // args : cat - input.txt NULL
        execute_other_command(args,  input_fd,  output_fd);
    }
    free(line);
    return 0;
}
