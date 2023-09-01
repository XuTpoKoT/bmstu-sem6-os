#include <stdio.h>
#include <fcntl.h>
#include <pthread.h>

void *task(void *fd) {
  int flag = 1;
  char c;

  FILE *fs = fdopen((int)fd,"r");
  char buff[20];
  setvbuf(fs, buff, _IOFBF, 20);

  while (flag == 1) {
    flag = fscanf(fs,"%c",&c);
    if (flag == 1)
      fprintf(stdout,"%c",c);
  }
}

int main(){
  int fd = open("alphabet.txt", O_RDONLY);

  pthread_t thids[2];
  pthread_create(thids,NULL,task,(void *)fd);
  pthread_create(thids+1,NULL,task,(void *)fd);

  pthread_join(thids[0],NULL);
  pthread_join(thids[1],NULL);
  return 0;
}
