#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

int dopath(char *filename, int depth);

int main(int argc, char * argv[])
{
    int code_error = 0;
    if (argc == 2)
        code_error = dopath(argv[1], 0);
    else
    {
        printf("Use ./a.out <base_dir>\n");
        code_error = -1;
    }
    return code_error;
}

int dopath(char *filename, int depth)
{
    int ind = -1;
    int isContinue = 0;
    DIR* treeDir[100];
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

        for (int i = 0; i < depth; i++)
        {
            printf("|   ");
        }

        if (S_ISDIR(statbuf.st_mode) == 0) // не каталог
        {
            printf("-- %s\n", filename);
        }
        else 
        {
            printf("\033[0;32m%s\033[0;0m\n", filename);

            DIR* dp = opendir(filename);
            if (dp == NULL) // каталог недоступен
                return -3;
            chdir(filename);
            ind++;
            depth++;
            treeDir[ind] = dp;
        }

        isContinue = 0;
        struct dirent *dirp;
        while (!isContinue && ind >= 0)
        {
            while (!isContinue && ((dirp = readdir(treeDir[ind])) != NULL))
            {
                if (strcmp(dirp->d_name, ".") && strcmp(dirp->d_name, ".."))
                {
                    filename = dirp->d_name;
                    isContinue = 1;
                }
            }
            if (isContinue)
            {
                continue;
            }
            chdir("..");
            depth--;

            if (closedir(treeDir[ind]) < 0)
            {
                printf("Can`t close dir");
                code_error = -4;
            }
            //printf("CLOSE %d %d %s\n", depth, ind, filename);
            ind--;
        }
    }
    

    return 0;
}