//mywc.c
//Iqbal:Arsum:A00351433
//Lab 01
//Stripped Down version of "wc"

#include <stdio.h>

int main(int argc, char *argv[])
{
    int lineCount, wordCount, byteCount;
    int isWord = 0;
    if(argc != 2)
    {
        printf("Not enough command line arguments\n");
        return 1;
    }
    else
    {
        FILE *fp = fopen(argv[1],"r");
        if(!fp)
        {
            printf("file doesnt exist\n");
            return 2;
        }
        else
        {
            wordCount = 0;
            int ch = 0;
            do
            {
                ch = fgetc(fp);
                if(!isspace(ch))
                    isWord = 1;
                if(isspace(ch) && isWord == 1)
                {
                    isWord = 0;
                    wordCount++;
                }
                if(ch == '\n')
                    lineCount++;
                if(ch != EOF)
                    byteCount++;
            }
            while(ch != EOF);
        }
        printf(" %d %d %d %s \n", lineCount, wordCount, byteCount, argv[1]);
        fclose(fp);
    }
    return 0;
}
