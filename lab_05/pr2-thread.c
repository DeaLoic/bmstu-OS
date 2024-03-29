#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <fcntl.h>

void *thread_read(int *fd)
{
    char c;
    while (read(*fd, &c, 1) == 1)
    {
        write(1, &c, 1);
    }
}

int main()
{
    int fd1 = open("alphabet.txt", O_RDONLY);
    int fd2 = open("alphabet.txt", O_RDONLY);

    pthread_t thread1, thread2;
    if (pthread_create(&thread1, NULL, thread_read, &fd1) != 0)
    {
        printf("Error. Can`t create thread 1!\n");
        exit(1);
    }

    if (pthread_create(&thread2, NULL, thread_read, &fd2) != 0)
    {
        printf("Error. Can`t create thread 2!\n");
        exit(1);
    }
    
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    return 0;
}