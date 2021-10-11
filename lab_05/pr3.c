#include <stdio.h>
#include <sys/stat.h>
#include <string.h>

#define fileName "alphabet2.txt"
int main()
{
  struct stat sb;

  FILE *fp1 = fopen(fileName, "w");
  stat(fileName, &sb);
  printf("Fp1. inode = %d   size = %d\n", sb.st_ino, sb.st_size);

  FILE *fp2 = fopen(fileName, "w");
  stat(fileName, &sb);
  printf("Fp2. inode = %d   size = %d\n", sb.st_ino, sb.st_size);

  // read a char & write it alternatingly from fs1 and fs2
  char* alphabet = "abcdefgh";
  int i = 0;
  int len = strlen(alphabet);
  while (i < len)
  {
    fprintf(fp1, "%c", alphabet[i]);
    fprintf(fp2, "%c", alphabet[i + 1]);
    i += 2;
  }

  fclose(fp1);
  stat(fileName, &sb);
  printf("Fp1. inode = %d   size = %d\n", sb.st_ino, sb.st_size);

  fclose(fp2);
  stat(fileName, &sb);
  printf("Fp2. inode = %d   size = %d\n", sb.st_ino, sb.st_size);


  return 0;
}