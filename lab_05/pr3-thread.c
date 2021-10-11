#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>

#define fileName "alphabet2.txt"

void *thread_write(int num)
{
    struct stat sb;

    FILE *fp = fopen(fileName, "w");
    stat(fileName, &sb);
    printf("Fp%d. inode = %d   size = %d\n", num, sb.st_ino, sb.st_size);

    char *alphabet = "abcdefgh";
    int i = 0;
    int len = strlen(alphabet);
    while (i < len)
    {
        if (num == 1)
        {
            fprintf(fp, "%c", alphabet[i]);
        }
        else
        {
            fprintf(fp, "%c", alphabet[i + 1]);
        }
        i += 2;
    }

    fclose(fp);
    stat(fileName, &sb);
    printf("Fp%d. inode = %d   size = %d\n", num, sb.st_ino, sb.st_size);
}

int main()
{
    int thread_num = 1;
    int fd2 = 2;

    pthread_t thread1, thread2;
    if (pthread_create(&thread1, NULL, thread_write, 1) != 0)
    {
        printf("Error. Can`t create thread 1!\n");
        exit(1);
    }

    if (pthread_create(&thread2, NULL, thread_write, 2) != 0)
    {
        printf("Error. Can`t create thread 2!\n");
        exit(1);
    }

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    return 0;
}