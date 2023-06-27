#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>

void log_exit(char *msg)
{
  perror(msg);
  exit(1);
}

int main()
{
  const int cnt_childs = 3;
  const int msg_maxl = 4;
  const char *msg_to_pr[3] = {
    "aaaa",
    "bbbb",
    "cccc"
  };
  const char *msg_to_ch[3] = {
    "1111",
    "2222",
    "3333"
  };
  int sock[2];

  char buf[5];
  buf[4] = '\0';
  pid_t pid[cnt_childs];

  if (socketpair(AF_UNIX, SOCK_DGRAM, 0, sock) == -1)
      log_exit("cant socketpair");

  for (int i = 0; i < cnt_childs; i++)
  {
    if ((pid[i] = fork()) == -1)
      log_exit("cant fork");
    if (pid[i] == 0)
    {
      printf("child %d write: %s\n", getpid(), msg_to_pr[i]);
      write(sock[1], msg_to_pr[i], msg_maxl);
      read(sock[1], buf, msg_maxl);
      printf("child %d recieve: %s\n", getpid(), buf);
      return 0;
    }
    read(sock[0], buf, msg_maxl);
    printf("parent recieve: %s from child %d\n", buf, pid[i]);
    write(sock[0], msg_to_ch[i], msg_maxl);
    printf("parent write: %s\n", msg_to_ch[i]);
  }

  return 0;
}
