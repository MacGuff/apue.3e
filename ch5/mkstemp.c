#include "apue.h"
#include <errno.h>

void make_temp(char *template);

int
main()
{
    char good[] = "/tmp/dirXXXXXX";
    char *bad = "/tmp/dirXXXXXX";

    printf("trying to create first file...");
    make_temp(good);
    printf("trying to create second file...");
    make_temp(bad);
    return 0;
}

void
make_temp(char *template)
{
    int fd;
    struct stat sbuf;

    if ((fd = mkstemp(template)) < 0)
        err_sys("can't create temp file");
    printf("temp name = %s\n", template);
    close(fd);

    if (stat(template, &sbuf) < 0) {
        if (errno == ENOENT)
            printf("file doesn't exist\n");
        else
            err_sys("stat failed");
    } else {
        printf("file exists\n");
        unlink(template);
    }
}
