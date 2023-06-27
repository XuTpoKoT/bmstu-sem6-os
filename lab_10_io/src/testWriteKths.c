#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <sys/stat.h>

struct stat statbuf;

#define PRINT_STAT(action) \
    do { \
        stat("q.txt", &statbuf); \
        fprintf(stdout, action ": inode number = %ld, \
                     size = %ld bytes, blksize = %ld\n", \
                    statbuf.st_ino, statbuf.st_size, \
                    statbuf.st_blksize); \
    } while (0)

void *thread_func(void *fd)
{
    for (char c = 'a'; c <= 'm'; c++)
    {
        write((int)fd, &c, 1);
        PRINT_STAT("write");
    }

    return NULL;
}

void *thread_func2(void *fd)
{
    for (char c = 'n'; c <= 'z'; c++)
    {
        write((int)fd, &c, 1);
        PRINT_STAT("write");
    }

    return NULL;
}

int main(void)
{
    int fd[2] = {open("q.txt", O_CREAT | O_WRONLY | O_APPEND, 0642), 
                 open("q.txt", O_CREAT | O_WRONLY | O_APPEND, 0642)};
    pthread_t threads[2];

    void *(*thread_funcs[2])(void *) 
                    = {thread_func, thread_func2};

    for (size_t i = 0; i < 2; i++)
        if (pthread_create(&threads[i], NULL,
                    thread_funcs[i], fd[i]) != 0)
        {
            perror("pthread_create\n");
            exit(1);
        }

    for (size_t i = 0; i < 2; i++)
        if (pthread_join(threads[i], NULL) != 0)
        {
            perror("pthread_join\n");
            exit(1);
        }

    close(fd[0]);
    close(fd[1]);

    return 0;
}
