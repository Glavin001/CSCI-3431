//average.c
//Iqbal:Arsum:A00351433
//Lab 06
//Find average using 10 processes

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>

FILE *fp;

int main (int argc, char **argv)
{
    key_t key = 1993;
    int shmid, i;
    pid_t pid;
    char *shm;
    double *values;
 
    //Get shared memory segment
    if ((shmid = shmget(key, 1000, IPC_CREAT | 0666)) < 0)
    {
        fprintf(stderr, "Failed to get memory!\n");
        exit(EXIT_FAILURE);
    }

    //Attach shared memory segment
    if ((values = shmat(shmid, NULL, 0)) == (double *) -1) 
    {
        fprintf(stderr, "Failed to attach memory!\n");
        exit(EXIT_FAILURE);
    }
    
    //Clear the shared memory
    for (i = 0; i < 2; i++) values[i] = 0;

    //If file doesnt open
    if (!(fp = fopen(argv[1], "r")))
    {
        fprintf(stderr, "Failed to open file %s\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    //Create 10 children
    for (i = 0; i < 10; i++)
    {
      if ((pid = fork()) == -1)
      {
          fprintf(stderr, "Failed to fork!\n");
          exit(EXIT_FAILURE);
      }
      else if (pid == 0)
          return (doStuff (i,values));
    }

    //Wait till all children finishes
    while (1)
    {
        //Wait for the 'flag' to be set
        if(values[10] == 1)
        {
            double sum = 0;
            //Sum up the "sums" calculated from each of the 10 processes 
            for (i = 0; i < 10; i++)    
                sum += values[i];
            double average = sum/10;
            printf("Average of the file \"%s\" is \"%f\"\n", argv[1], average);
            fclose(fp);
            exit(EXIT_SUCCESS);
        }
    }
}

int doStuff(int processNum, double *values)
{
    double lineValue, sum = 0;
    int i;
    //Choose where to start reading from
    fseek(fp, 1000*processNum, SEEK_SET);
    //Read 100 lines and sum them up
    for (i = 0; i < 100; i++) 
    {
        fscanf(fp, "%lf\n", &lineValue);
        sum += lineValue;
    }
    //Store summed values in shared memory 
    values[processNum] = sum;
    //Set the 'flag' value when the last process finishes
    if(processNum == 9)
        values[10] = 1;
    exit(EXIT_SUCCESS);
}
