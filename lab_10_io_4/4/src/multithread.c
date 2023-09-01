#include <fcntl.h>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>

#define PRINT_STAT(path, prefix) \
  do { \
    stat(path, &statbuf); \
    fprintf(stdout, prefix ": inode num = %ld, size = %ld, blksize = %ld\n", \
    statbuf.st_ino, statbuf.st_size, \
    statbuf.st_blksize); \
  } while (0)

void *task(void *payload){
  int n = (int)payload;
  int file = open("output.txt", O_WRONLY | O_APPEND);
  struct stat statbuf;

  for (char ch = 'a'; ch <= 'z'; ch++)
  if (ch % 2 == n) {
    write(file, &ch, 1);
    PRINT_STAT("output.txt", "write");
  }
  close(file);
  return 0;
}

int main() {
  pthread_t thids[2];
  pthread_create(thids,NULL,task,(void *)0);
  pthread_create(thids+1,NULL,task,(void *)1);
  pthread_join(thids[0],NULL);
  pthread_join(thids[1],NULL);
  return 0;
}
