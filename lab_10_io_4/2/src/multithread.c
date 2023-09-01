#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>

void *task() {
  int fd = open("alphabet.txt",O_RDONLY);
  int flag = 1;
  char c;

  while (flag) {
    if ((flag=read(fd,&c,1)))
      write(1,&c,1);
  }
}

int main(){
  pthread_t thids[2];
  pthread_create(thids,NULL,task,NULL);
  pthread_create(thids+1,NULL,task,NULL);

  pthread_join(thids[0],NULL);
  pthread_join(thids[1],NULL);
  return 0;
}
