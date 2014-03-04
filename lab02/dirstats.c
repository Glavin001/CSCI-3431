//dirstats.c
//Iqbal:Arsum:A00351433
//Lab 02
//fstat for directories

#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <dirent.h>
#include <string.h>

int main(int argc, char *argv[])
{
    //create char array to use later for filename when using strcpy/strcat
    char filename[500];
    struct stat fileStats;
    if(argc != 2)
    {
        printf("Not enough command line arguments\n");
        return 1;
    }
    else
    {
        //create a directory pointer to use for opendir
        DIR *dp = NULL;
        //create a dirent pointer to use for readdir
        struct dirent *dptr = NULL;
        //if opendir returns null, it means directory didnt open
        if(NULL == (dp = opendir(argv[1])))
        {
            printf("Cannot open directory: %s\n",argv[1]);
            return 2;
        }
        else
        {
            printf("Directory: %s\n", argv[1]);
            //keep going until the dptr reaches null at the end
            while(NULL != (dptr = readdir(dp)) )
            {
                const char *dirname = dptr->d_name;
                //copy our directory from argument into filename array
                strcpy(filename, argv[1]);
                //add / to keep the path going
                strcat(filename,"/");
                strcat(filename,dirname);
                //populate the structure, if it fails print error
                if(stat(filename, &fileStats) == -1)
                {
                    printf("Error");
                    return 3;                  
                }
                //remove the folder with name "." and ".."
                if((strcmp(".",dptr->d_name) != 0) && (strcmp("..",dptr->d_name) != 0))
                {
                    //print information for each file
                    printf("  File: %s\n", dptr->d_name);
                    printf("    File size: %lld bytes\n", (long long)fileStats.st_size);
                    printf("    Last modification: %s", ctime(&fileStats.st_mtime));
                }
            }
        }
    }
    return 0;
}
