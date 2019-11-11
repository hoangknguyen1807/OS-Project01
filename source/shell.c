#include <malloc.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#define MAXLINE 80  /* The maximum length of a command */
#define MAXWORD 20  /* The maximum length of an argument*/

/* file descriptor for input, output redirection */
int fdi, fdo;
int saved_stdin, saved_stdout;

char* args[MAXLINE / 2 + 1]; /* command line arguments */
int nArgs; /* number of arguments */


void stringTokenizer(char* tokens[], char* source, char* delim, int *nWords)
{
    char *ptr = strtok(source, delim);
    int count = 0;
    while (ptr != NULL)
    {
        strcpy(tokens[count], ptr);
        count++;
        ptr = strtok(NULL, delim);
    }
    *nWords = count;
    return;
}

void enterCommandLine(char command[])
{
    fgets(command, MAXLINE, stdin);
    short int len = strlen(command);
    command[len - 1] = 0;
}

void allocateMemory(char* argArray[])
{
    for (int i = 0; i < MAXLINE / 2 + 1; i++)
    {
        argArray[i] = (char *)malloc(sizeof(char) * MAXWORD);
    }
}

void freeMemory(char* argArray[])
{
    for (int i = 0; i < MAXLINE / 2 + 1; i++)
    {
        if (argArray[i] != NULL)
        {
            free(argArray[i]);
            argArray[i] = NULL;
        }
    }
}

void handle_output_redirect()
{
    saved_stdout = dup(STDOUT_FILENO);
    fdo = open(args[nArgs - 1], O_CREAT | O_WRONLY | O_TRUNC);
    dup2(fdo, STDOUT_FILENO);
    free(args[nArgs - 2]);
    args[nArgs - 2] = NULL;
}

void restore_STDOUT()
{
    close(fdo);
    dup2(saved_stdout, STDOUT_FILENO);
    close(saved_stdout);
}

void handle_input_redirect()
{
    saved_stdin = dup(STDIN_FILENO);
    fdi = open(args[nArgs - 1], O_RDONLY);
    dup2(fdi, STDIN_FILENO);
    free(args[nArgs - 2]);
    args[nArgs - 2] = NULL; 
}

void restore_STDIN()
{
    close(fdi);
    dup2(saved_stdin, STDIN_FILENO);
    close(saved_stdin);
}

// Function where the piped system commands is executed
void execArgsPiped(char *parsed[], char *parsedpipe[])
{
    int stat;
    // 0 is read end, 1 is write end
    int pipefd[2];
    pid_t pid1, pid2;

    pid1 = fork();
    if (pid1 < 0)
    {
        printf("\nCould not fork");
        return;
    }

    if (pid1 == 0)
    {
        if (pipe(pipefd) < 0)
        {
            printf("\nPipe could not be initialized");
            return;
        }
        pid2 = fork();

        if (pid2 < 0)
        {
            printf("\nCould not fork");
            exit(0);
        }

        // Child 2 executing..
        // It only needs to read at the read end
        if (pid2 == 0)
        {
            // close read end of pipe
            close(pipefd[0]);
            // duplicate write end of pipe to stdout
            dup2(pipefd[1], STDOUT_FILENO);
            
            if (execvp(parsed[0], parsed) < 0)
            {
                printf("\nCould not execute command 1..");
                exit(0);
            }
        }
        else
        // Child 1 executing..
        // It only needs to write at the write end
        {
            //while (wait(&stat) != pid2);
            wait(NULL);
            //close write end of pipe
            close(pipefd[1]);
            //duplicate read end of pipe to stdin
            dup2(pipefd[0], STDIN_FILENO);
            
            if (execvp(parsedpipe[0], parsedpipe) < 0)
            {
                printf("\nCould not execute command 2..");
                exit(0);
            }
        } 
    }
    else
    {
        // parent executing, waiting for two children
        //while (wait(&stat) != pid1);
        wait(NULL);
    }

}

// function for finding pipe symbol
int parsePipe(char *cmd, char* cmdpiped[])
{
    int i;
    for (i = 0; i < 2; i++)
    {
        cmdpiped[i] = strsep(&cmd, "|");
        if (cmdpiped[i] == NULL)
            break;
    }

    if (cmdpiped[1] == NULL)
        return 0; // returns zero if no pipe is found.
    else
    {
        return 1;
    }
}

void parseSpace(char *cmd, char *parsedArg[])
{
    int i = 0;

    while (cmd != NULL)
    {
        parsedArg[i] = strsep(&cmd, " ");
        if (parsedArg[i][0] != 0)
            i++;
    }
    parsedArg[i] = NULL;
}

int processString(char *cmd, char *parsed[], char *parsedpipe[])
{
    char *cmdpiped[2];
    int piped = 0;

    piped = parsePipe(cmd, cmdpiped);

    if (piped)
    {
        parseSpace(cmdpiped[0], parsed);
        parseSpace(cmdpiped[1], parsedpipe);
    }

    return piped;
}

int main(void)
{
    printf("+ Nguyen Khanh Hoang - 1712457\n");
    printf("+ Pham Minh Hoang - 1712460\n");
    printf("+ Vu Duy Khanh - 1612866\n");
    printf("--------------------------------\n");
    printf(">>>>> Simple Shell Program <<<<<\n");
    printf("--------------------------------\n");
    sleep(3);
    system("clear");
    char command[MAXLINE]; // command entered by user
    char lastCommand[MAXLINE]; // save the most recent command
    strcpy(lastCommand, "\0\0");
    char* parsedArgs[MAXLINE / 2 + 1];  // first cmd of the pipe 
    char* parsedArgsPiped[MAXLINE / 2 + 1]; // second cmd of the pipe
    int pipeExec = 0;   // notify whether to execute a pipe or not
    pid_t pid; /* process id */
    int isRedirectOutput = 0, isRedirectInput = 0;
    
    while (1)
    {
        // in case user Enters without typing anything
        do
        {
            printf("osh>");
            fflush(stdout);
            enterCommandLine(command);
        } while (command[0] == 0);

        
        if (strcmp(command, "!!") == 0)
        {
            if (lastCommand[0] == 0)
            {
                printf("No commands in history!\n");
                continue;
            }
            else
            {
                printf("%s\n", lastCommand);
                strcpy(command, lastCommand);
            }
        }
        else
        {
            // save command user just entered
            strcpy(lastCommand, command);
        }
        
        // pipeExec returns 0 if it is a simple command
        // 1 if it is including a pipe.
        pipeExec = processString(command, parsedArgs, parsedArgsPiped);

        if (pipeExec == 0)
        {
            // allocate memory for the array of arguments
            allocateMemory(args);
            strcpy(command, lastCommand);
            // tokenize command into arguments
            stringTokenizer(args, command, " ", &nArgs);

            // the argument right after the last argument
            // must be NULL
            free(args[nArgs]);
            args[nArgs] = NULL;

            if (strcmp(args[0], "exit") == 0)
            {
                break;
            }
            else
            {
                if (strcmp(args[0], "~") == 0)
                {
                    char* homedir = getenv("HOME");
                    printf("Home directory : %s\n", homedir);
                }
                else // cd (change directory)
                if (strcmp(args[0], "cd") == 0)
                {
                    char dir[MAXLINE];
                    strcpy(dir, args[1]);
                    if (strcmp(dir, "~") == 0)
                    {
                        strcpy(dir, getenv("HOME"));
                    }
                    chdir(dir);
                    printf("Current directory : ");
                    getcwd(dir, sizeof(dir));
                    printf("%s\n", dir);                    
                }
                else
                {
                    // parentWait == 0 : last arg is "&", parent doesn't wait, runs concurrently
                    // parentWait != 0 : parent waits for child to finish
                    int parentWait = strcmp(args[nArgs - 1], "&");

                    if (parentWait == 0)
                    {
                        free(args[nArgs - 1]);
                        args[nArgs - 1] = NULL;
                    }

                    // output redirection
                    if ((nArgs > 1) && strcmp(args[nArgs - 2], ">") == 0)
                    {
                        handle_output_redirect();
                        isRedirectOutput = 1;
                    }
                    else
                        //input redirection
                        if ((nArgs > 1) && strcmp(args[nArgs - 2], "<") == 0)
                    {
                        handle_input_redirect();
                        isRedirectInput = 1;
                    }

                    pid = fork();

                    if (pid < 0)
                    {
                        fprintf(stderr, "FORK FAILED\n");
                        return -1;
                    }
                    if (pid == 0)
                    {

                        int ret = execvp(args[0], args);

                        if (ret == -1)
                        {
                            printf("Invalid command\n");
                        }
                        exit(ret);
                    }
                    else
                    {
                        if (parentWait)
                        {
                            while (wait(NULL) != pid)
                                ;
                            if (isRedirectOutput)
                            {
                                restore_STDOUT();
                                isRedirectOutput = 0;
                            }
                            if (isRedirectInput)
                            {
                                restore_STDIN();
                                isRedirectInput = 0;
                            }
                            //printf("Child process has terminated.\n");
                        }

                        // free allocated memory
                        freeMemory(args);
                    }
                }
            }
        }
        else
        {
            execArgsPiped(parsedArgs, parsedArgsPiped);
        }
        /**
        * After reading user input, the steps are:
        * (1) fork a child process using fork()
        * (2) the child process will invoke execvp()
        * (3) parent will invoke wait() unless command included &
        */
    }
    freeMemory(args);
    return 0;
}
