#include <stdio.h>

#define  BUF_SIZE 0x100

int main() {
    char buf[BUF_SIZE];
    int len, i;
    FILE *f;

    f = fopen("/proc/self/environ", "r");
    printf("asd\n");
    while ((len = fread(buf, 1, BUF_SIZE - 1, f)) > 0) {
        for (i = 0; i < len; i++) {
            if (buf[i] == 0) {
                buf[i] = '\n';
            }
        }
        buf[len] = 0;
        printf("%s", buf);
    }

    fclose(f);
    return 0;
}