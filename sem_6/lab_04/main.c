#include <stdio.h>
#include <string.h>

#define BASE "/proc/"

#define LEN 5
char* files[LEN] = {"cmdline", "environ", "maps", "stat", "statm"};

int writeFileToFile(char* pathSource, char* pathDest);

int main(int argc, char* argv[])
{
    char* pid = argv[1];
    char* destFile = argv[2];
    char basePath[100];
    sprintf(basePath, "%s%s/", BASE, pid);
    FILE* f = fopen(destFile, "w");
    fclose(f);
    for (int i = 0; i < LEN; i++)
    {
        char curFile[200];
        sprintf(curFile, "%s%s", basePath, files[i]);
        printf("%s  %s\n", curFile, destFile);
        
        writeFileToFile(curFile, destFile);
    }
}

int writeFileToFile(char* pathSource, char* pathDest)
{
    FILE *source = fopen(pathSource, "r");
    FILE *dest = fopen(pathDest, "a+");

    char buf[100];
    fprintf(dest, "\n\n%s\n", pathSource);
    
    while (!feof(source))
    {
        int size = fread(buf, 1, 100, source);
        for (int i = 0; i < size; i++)
        {
            if (buf[i] == '\0')
            {
                buf[i] = '\n';
            }
        }
        fwrite(buf, 1, size, dest);
    }

    fclose(source);
    fclose(dest);
}