#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>

#define PRINT_STAT(path, action) \
  do { \
    stat(path, &statbuf); \
    fprintf(stdout, action ": inode num = %ld, size = %ld, blksize = %ld\n", \
    statbuf.st_ino, statbuf.st_size, \
    statbuf.st_blksize); \
  } while (0)

int main() {
  struct stat statbuf;
  FILE *file1 = fopen("output.txt", "w");
  PRINT_STAT("output.txt", "fopen file1");
  FILE *file2 = fopen("output.txt", "w");
  PRINT_STAT("output.txt", "fopen file2");

  for (char ch='a'; ch<='z'; ch++) {
    if (ch % 2)
      fprintf(file1,"%c",ch);
    else
      fprintf(file2,"%c",ch);
    PRINT_STAT("output.txt", "fprintf");
  }
  
  fclose(file1);
  PRINT_STAT("output.txt", "fclose file1");
  fclose(file2);
  PRINT_STAT("output.txt", "fclose file2");
  return 0;
}
