#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>

#define PRINT_STAT(path, prefix) \
  do { \
    stat(path, &statbuf); \
    fprintf(stdout, prefix ": inode num = %ld, size = %ld, blksize = %ld\n", \
    statbuf.st_ino, statbuf.st_size, \
    statbuf.st_blksize); \
  } while (0)

int main() {
  struct stat statbuf;
  int file1 = open("output.txt", O_RDWR);
  PRINT_STAT("output.txt", "open file1");
  int file2 = open("output.txt", O_RDWR);
  PRINT_STAT("output.txt", "open file2");

  for (char ch = 'a'; ch <= 'z'; ch++) {
    if (ch % 2)
      write(file1, &ch, 1);
    else
      write(file2, &ch, 1);
    PRINT_STAT("output.txt", "write");
  }

  close(file1);
  PRINT_STAT("output.txt", "close file1");
  close(file2);
  PRINT_STAT("output.txt", "close file2");
  return 0;
}
