#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>

void* thread1(void *args)
{
    int *fd = (int *)args;
    FILE *fs1 = fdopen(*fd,"r");
	char buff1[20];
	setvbuf(fs1,buff1,_IOFBF,20);

    int flag = 1;
    char c;

    while ((flag = fscanf(fs1, "%c", &c)) == 1)
    {
        
		fprintf(stdout, "%c", c);
		
    }

    return NULL;
}

void* thread2(void *args)
{
    int *fd = (int *)args;
    FILE *fs2 = fdopen(*fd,"r");
	char buff2[20];
	setvbuf(fs2,buff2,_IOFBF,20);

    int flag = 1;
    char c;

    while ((flag = fscanf(fs2, "%c", &c)) == 1)
    {
		fprintf(stdout, "%c", c);
    }

    return NULL;
}

int main()
{
    pthread_t t1, t2;

    int fd = open("alphabet.txt", O_RDONLY);

    pthread_create(&t1, NULL, thread1, &fd);

    pthread_create(&t2, NULL, thread2, &fd);

    pthread_join(t1, NULL) ;
    pthread_join(t2, NULL) ;


    return 0;
}
