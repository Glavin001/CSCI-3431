#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>

#define SHMSIZE 1024
key_t key = 1993;
int shmid, count = 0;
int *shm;

sem_t   first,
        second;

void calculate(int process, int num);

int main(int argc, char **argv)
{
    int number,i;
    pid_t pid;
int count = 0;
    //0 = lock , 1 = unlock
    sem_init(&first,1,1);
    sem_init(&second,1,0);

    if(argc != 2)
    {
        fprintf(stderr,"Not enough command line arguments\n");
        exit(EXIT_FAILURE);
    }

    number = atoi(argv[1]);

    /* 1. GET a shared memory section */
    if ((shmid = shmget(key, SHMSIZE, IPC_CREAT | 0600)) < 0)
    {
        fprintf(stderr, "Error\n");
        exit(EXIT_FAILURE);
    }

    /* 2. ATTACH it to the process */
    if ((shm = shmat(shmid, NULL, 0)) == (int *) -1)
    {
        fprintf(stderr,"Error\n");
        exit(EXIT_FAILURE);
    }

    shm[0] = 1;
    shm[1] = 1;
    shm[2] = 0;
    //Create 2 children
    for (i = 1; i <= 2; i++)
    {
        if((pid = fork()) == -1)
        {
            fprintf(stderr, "Error\n");
            exit(EXIT_FAILURE);
        }
        if(pid == 0)
        {
            calculate(i, number);    
        }
    } 
      // sem_destroy(&first);
       //sem_destroy(&second);
        exit(EXIT_SUCCESS);

}

void calculate(int process, int num)
{
    int i;
    for(i = 0; i < num - 1; i++)
    {
        if(process == 1){
            //sem_wait(&first);
            while(1){
                if(shm[2] == 0){
                    int temp1 = shm[0] + shm[1];
                    shm[0] = shm[1];
                    shm[1] = temp1;
                    printf("Process(%d) %d: fibonacci number is %d.\n", process, getpid(), temp1);
                    shm[2] = 1;
goto tester;
                }
            }
  //          sem_post(&second);
        }
        if(process == 2){
            while(1){
                if(shm[2] == 1){
            //sem_wait(&second);
                    int temp2 = shm[0] + shm[1];
                    shm[0] = shm[1];
                    shm[1] = temp2;
                    printf("Process(%d) %d: fibonacci number is %d.\n", process, getpid(), temp2);
                    shm[2] = 0;
goto tester;
                }
            }
//    sem_post(&first);
        }
    tester:
        printf("h");
    }
    exit(EXIT_SUCCESS);
}
