#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h> 
#include "stack.h"

int dopath(char *filename, int depth);

int main(int argc, char * argv[])
{
    int code_error = 0;
    if (argc == 2)
        code_error = dopath(argv[1], 0);
    else
    {
        printf("Pass target dir\n");
        code_error = -1;
    }
    return code_error;
}

int dopath(char *filename, int depth)
{
    // WORK
    int ind = -1;
    int isContinue = 0;
    int error = 0;
    stack_t stack;
    set_null_stack(&stack);
    depth = 1;
    while (depth > 0)
    {
        if (ind == -1)
        {
            depth = 0;
        }
        int code_error = 0;

        struct stat statbuf;
        if (lstat(filename, &statbuf) < 0) // ошибка
        {
            printf("ERROR lstat");
            return -1;
        }

        for (int i = 0; i < stack.size; i++)
        {
            printf("|   ");
        }

        if (S_ISDIR(statbuf.st_mode) == 0) // не каталог
        {
            printf("-- %s\n", filename);
        }
        else 
        {
            printf("%s\n", filename);

            DIR* dp = opendir(filename);
            if (dp == NULL) // каталог недоступен
                return -3;
            chdir(filename);
            error = push_stack(&stack, (void*)dp);
            if (error)
            {
                printf("ERROR STACK PICK^ %d\n", error);
                return error;
            }
            ind++;
            depth++;
        }
        DIR* dp;
        isContinue = 0;
        struct dirent *dirp;
        while (!isContinue && ind >= 0)
        {
            error = peek_stack(&stack, &dp);
            if (error)
            {
                printf("ERROR STACK PICK^ %d\n", error);
                return error;
            }
            while (!isContinue && ((dirp = readdir(dp)) != NULL))
            {
                if (strcmp(dirp->d_name, ".") && strcmp(dirp->d_name, ".."))
                {
                    filename = dirp->d_name;
                    isContinue = 1;
                }
            }
            if (!isContinue)
            {
                chdir("..");
                depth--;

                if (closedir(dp) < 0)
                {
                    printf("Can`t close dir");
                    code_error = -4;
                }
                error = pop_stack(&stack);
                if (error)
                {
                    printf("ERROR STACK POP^ %d\n", error);
                    return error;
                }
                //printf("CLOSE %d %d %s\n", depth, ind, filename);
                ind--;
            }
        }
    }
    

    return 0;
}