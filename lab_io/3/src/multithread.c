#include <stdio.h>
#include <fcntl.h>
#include <pthread.h>

void *task(void *payload) {
  int n = (int)payload;
  FILE *file = fopen("output.txt", "w");
  for (char ch = 'a'; ch <= 'z'; ch++) {
    if (ch % 2 == n)
      fprintf(file,"thread_ind = %d: char = %c\n", n, ch);
  }
  fclose(file);
  return 0;
}

int main() {
  pthread_t thids[2];
  pthread_create(thids, NULL, task, (void *)0);
  pthread_create(thids+1, NULL, task, (void *)1);
  pthread_join(thids[0],NULL);
  pthread_join(thids[1],NULL);
  return 0;
}
