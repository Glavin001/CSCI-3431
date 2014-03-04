/*
 * lab9: merge.c
 * Arsum Iqbal - A00351433
 */

#include <stdio.h>
#include <sys/types.h>
#include <errno.h> 
#include <string.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>

FILE *file1;
FILE *file2;
FILE *file3;

sem_t   first,
        second;

void* processFile1(void* name)
{

    char line[80];
    char *fileName = name;
    int lineCount = 1;
    //Get each line from the first file(file1) and store it in "line"
    while(fgets(line, 80, file1))
    {        
        /* Critical Section */
        //Wait to get unlocked
        sem_wait(&first);
        //Print to the third file
        fprintf(file3,"%s: %d: %s", fileName,lineCount,line);
        lineCount++;
        //Unlock second function
        sem_post(&second);
    }
    //Close the first file
    fclose(file1);
    //Exit pthread
    pthread_exit(0);
}

void* processFile2(void* name)
{

    char line[80];
    char *fileName = name;
    int lineCount = 1;
    //Get each line from the second file(file2) and store it in "line"
    while(fgets(line, 80, file2))
    {
        /* Critical Section */
        //Wait to get unlocked
        sem_wait(&second);
        //Print to the third file
        fprintf(file3,"%s: %d: %s", fileName,lineCount,line);
        lineCount++;
        //Unlock first function
        sem_post(&first);
    }
    //Close the second file
    fclose(file2);
    //Exit pthread
    pthread_exit(0);
}

int main(int argc, char **argv)
{
    pthread_t       tid,tid2;
    pthread_attr_t  attr,attr2;

    //0 = lock , 1 = unlock
    sem_init(&first,0,1);
    sem_init(&second,0,0);
    
    //Standard Error checking to see enough args
    if(argc != 4)
    {
        fprintf(stderr,"Not enough command line arguments!\n");
        exit(EXIT_FAILURE);
    }
    
    //Error checking to see if file 1 opens
    file1 = fopen(argv[1],"r");
    if(!file1)
    {
        fprintf(stderr,"Error: Could not open file: %s.\n", argv[1]);
        exit(EXIT_FAILURE);
    }
    
    //Error checking to see if file 2 opens
    file2 = fopen(argv[2],"r");
    if(!file2)
    {
        fprintf(stderr,"Error: Could not open file: %s.\n", argv[2]);
        exit(EXIT_FAILURE);
    }

    //Error checking to see if file 3 opens
    file3 = fopen(argv[3],"w");
    if(!file3)
    {
        fprintf(stderr,"Error: Could not open file: %s.\n", argv[3]);
        exit(EXIT_FAILURE);
    }

    //Initialize first thread and pass it the first file
    pthread_attr_init(&attr);
    pthread_create(&tid, &attr, processFile1,argv[1]);

    //Initialize second thread and pass it the second file
    pthread_attr_init(&attr2);
    pthread_create(&tid2, &attr2, processFile2,argv[2]);

    //Wait for both threads to finish
    pthread_join(tid, NULL);
    pthread_join(tid2, NULL);

    //Close the third file and destroy the semaphores
    fclose(file3);
    sem_destroy(&first);
    sem_destroy(&second);   

    return 0;
}
