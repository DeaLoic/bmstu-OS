#include <stdio.h>

int main()
{
    unsigned long int a = 1;
    for (int i = 1; i < 10000000; i++)
    { 
        a *= i;
        printf("Факториал число: %d\n\n", a);
    }
    printf("Пока!");
}