#include <fcntl.h>

int main()
{
    char c;
    int fd1 = open("alphabet.txt", O_RDONLY);
    int fd2 = open("alphabet.txt", O_RDONLY);

    int isReadCorrectly = 1;
    while (isReadCorrectly)
    {
        isReadCorrectly = 0;
        if (read(fd1, &c, 1) == 1)
        {
            write(1, &c, 1);
            if (read(fd2, &c, 1) == 1)
            {
                write(1, &c, 1);
                isReadCorrectly = 1;
            }
        }
    }
    return 0;
}