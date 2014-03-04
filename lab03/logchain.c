//logchain.c
//Iqbal:Arsum:A00351433
//Lab 03
//Forking

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

int main(int argc, char **argv)
{
    pid_t pid;
    int i, chain;
    FILE *fileName = fopen("lab3.log","w+");
    if(!fileName)
    { 
        fprintf(stderr,"Error : Could not open file.\n");
        exit(EXIT_FAILURE);
    }
    if(argc != 2)
    {
        fprintf(stderr,"Not enough command line arguments\n");
        exit(EXIT_FAILURE);
    }
    chain = atoi(argv[1]);
    printf("Creating chain of %d processes.\n", chain);
    for(i = 1; i <= chain; i++)
    {
        fflush(fileName);
        if ((pid = fork()) == -1)
        {
            fprintf(stderr, "Can't fork, error %d\n", errno);
            exit(EXIT_FAILURE);
        }
        //In the Child
        if(pid == 0) fprintf(fileName,"PID %d Starting\n", getppid());
        //In the parent
        else
        {
            wait(0);
            fprintf(fileName,"PID %d Ending\n", getpid());
            exit(0);
        }
    }
    fclose(fileName);
    exit(EXIT_SUCCESS);
}
