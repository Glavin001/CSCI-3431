/*
 * lab8: sort.c
 * Tami Meredith
 * Edited by: Arsum Iqbal - A00351433
 */

#include <stdio.h>
#include <sys/types.h>
#include <errno.h> 
#include <string.h>
#include <stdlib.h>

//Since threads only take 1 argument, we need to create a struct to hold args
typedef struct
{
    int *array;
    int first;
    int last;
} args;


/*
 * swap the values at addresses a and b
 */
void swap(int *a, int *b)
{
    int t=*a;
    *a=*b;
    *b=t;
}

/*
 * qsort arr in range beg..end inclusive
 */
//void sort(int arr[], int beg, int end)
void sort(void *a)
{
    int piv, l, r;

    //Setup since we need 2 threads, 1 for left and 1 for right recursion
    pthread_t  tid,tid2;
    pthread_attr_t attr,attr2;
    pthread_attr_init(&attr);
    pthread_attr_init(&attr2);
    
    //declare structs for left/right recursion
    args b,c;

    //Get the value from the struct and store it in their respective variables
    int *arr = ((args *) a)->array;
    int beg = ((args *) a)->first;
    int end = ((args *) a)->last;
    
    if (end > beg + 1)
    {
        piv = arr[beg], l = beg + 1, r = end;
        while (l < r)
        {
            if (arr[l] <= piv)
                l++;
            else
                swap(&arr[l], &arr[--r]);
        }
        swap(&arr[--l], &arr[beg]);

        //sort(arr, beg, l)
        //Assign the new values to the left side struct
        b.array = arr;
        b.first = beg;
        b.last = l;
        //Start the thread with the left side struct
        pthread_create(&tid, &attr, sort,&b);

        //sort(arr, r, end)
        //Assign the new values to the right side struct
        c.array = arr;
        c.first = r;
        c.last = end;
        //Start the thread with the right side struct
        pthread_create(&tid2, &attr2, sort,&c);
        
        //Wait for both threads to finish
        pthread_join(tid, NULL);
        pthread_join(tid2, NULL);
    }
    
}

/*
 * Print the data
 * m = label to prefix output
 * d = data, n = number of items in d
 */
void dprint(char *m, int *d, int n)
{
    int i;
    printf("%s: %d", m, d[0]);
    for (i = 1; i < n; i++)
    {
        printf(", %d", d[i]);
    }
    printf("\n");
}   

int main(int argc, char **argv)
{
    int data[256];
    int num, count, i;

    //Setup the thread
    pthread_t  tid;
    pthread_attr_t attr;
    pthread_attr_init(&attr);

    //Declare a struct to use so we can pass it into thread
    args a;

    //See if there are proper command line arguments
    if(argc != 2)
    {
        fprintf(stderr,"Not enough command line arguments!\n");
        exit(EXIT_FAILURE);
    }

    //Open file and test for errors
    FILE *fileName = fopen(argv[1],"r");
    if(!fileName)
    {
        fprintf(stderr,"Error: Could not open file.\n");
        exit(EXIT_FAILURE);
    }

    //Get the first number from the file and store it in count
    fscanf(fileName, "%d", &num);
    count = num;

    //Start storing the rest of the numbers from the file into the data array
    i = 0;
    while (fscanf(fileName, "%d", &num) == 1)
    {
        data[i] = num;
        i++;
    }

    //Print results
    dprint("Unsorted data", data, count);

    //Initially setup the struct so we can send it to our sort function
    a.array = data;
    a.first = 0;
    a.last = count;
    
    //Create our thread and pass in our struct
    pthread_create(&tid, &attr, sort,&a);    

    //Wait for thread to finish
    pthread_join(tid, NULL);

    //Print sorted results
    dprint("Sorted data", data, count);

    return (0);
}
