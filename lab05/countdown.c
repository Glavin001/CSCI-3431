//countdown.c
//Iqbal:Arsum:A00351433
//Lab 05
//Process Synchronization

#include <sys/types.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>   
#include <string.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <sys/ipc.h>

#define FLAGS   0
#define MSGTYPE 0

typedef struct{
    long msgtype;
    int data;
}value;
   
int mboxID[10];
int pmboxID;
value m;

void doStuff(int i, int *mboxID, int processNum);

int main(int argc, char **argv)
{    
    //If not enough command line arguments
    if(argc != 3)
    {
        fprintf(stderr,"Not enough command line arguments\n");
        exit(EXIT_FAILURE);
    }

    //If num_procs is not between 2 - 10
    int processNum = atoi(argv[1]);    
    if(processNum < 2 || processNum > 10)
    {
        fprintf(stderr,"Processes must be between 2 to 10!\n");
        exit(EXIT_FAILURE);
    }

    //If start value isnt between num_procs and 100
    int start = atoi(argv[2]);
    if(start < processNum || start > 100)
    {
        fprintf(stderr,"Start value must be between %d and 100!\n",processNum);
        exit(EXIT_FAILURE);
    }

    int i;
    //Create 'processNum' amounts of mailboxes
    for(i = 0; i < processNum; i++)
        mboxID[i] = msgget(IPC_PRIVATE, 0600 | IPC_CREAT);

    //Create mailbox for parent
    pmboxID = msgget(IPC_PRIVATE, 0600 | IPC_CREAT);
   
    m.msgtype = 1;
    //Initially send start value into mailbox
    m.data = start;
    msgsnd(mboxID[0], &m, sizeof(value), FLAGS);
    pid_t pid[10];

    //Create 'processNum' amounts of children 
    for(i = 0; i < processNum; i++)
    {   
        if((pid[i] = fork()) == -1)
        {
            fprintf(stderr,"Fork failed.\n");
            exit(EXIT_FAILURE);
        }
        if(pid[i] == 0)
            doStuff(i,mboxID,processNum);
    }

    //Force parent to wait for child to complete  
    msgrcv(pmboxID, &m, sizeof(value),MSGTYPE,FLAGS);  
    exit(EXIT_SUCCESS);
}

void doStuff(int i, int *mboxID, int processNum)
{
    while (1)
    {
        //Wait for message 
        msgrcv(mboxID[i], &m, sizeof(value),MSGTYPE,FLAGS);
        //If the value is 0, print it, send it to parent the exit
        if(m.data == 0)
        {
            printf("Process(%d) %d: integer is %d. Terminating\n", i+1, getpid(), m.data);
            msgsnd(pmboxID, &m,sizeof(value),FLAGS);
            exit(EXIT_SUCCESS);
        }
        //Otherwise print the value received
        printf("Process(%d) %d: integer is %d. ", i+1, getpid(), m.data);
        //If its the max processNumber
        if(i == (processNum-1))
        {
            //check if it the last one for itself
            if(m.data < processNum)
            {
                //Decrement value, print, send msg to the first process then quit
                m.data--;
                printf("Terminating\n");
                msgsnd(mboxID[0], &m, sizeof(value),FLAGS);
                exit(EXIT_SUCCESS);
            }
            //If the i'th process is not the last one for itself
            else
            {
                printf("\n");
                //Decrement value and send to first process
                m.data--;
                msgsnd(mboxID[0], &m, sizeof(value),FLAGS);
            }
        }
        //If its not the max processNumber
        else
        {
            //Check if its the last one for itself
            if(m.data < processNum)
            {
                //Decrement and send to next process
                m.data--;
                printf("Terminating\n");
                msgsnd(mboxID[i+1], &m, sizeof(value), FLAGS); 
                exit(EXIT_SUCCESS);
            }
            //Otherwise decrement and send to next process
            else
            {
                printf("\n");
                m.data--;
                msgsnd(mboxID[i+1], &m, sizeof(value), FLAGS);    
            }
        }
    }
}
