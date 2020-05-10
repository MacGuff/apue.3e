#include "apue.h"
#include <dirent.h>
#include <limits.h>

typedef int Myfunc(const char *, const struct stat *, int);

static Myfunc myfunc;
static int myftw(char *, Myfunc *);
static int dopath(Myfunc *);

#define FTW_F 1
#define FTW_D 2
#define FTW_DNR 3
#define FTW_NS 4

static char *fullpath;
static size_t pathlen;

static int
myftw(char *pathname, Myfunc *func)
{
    fullpath = path_alloc(&pathlen);

    if (pathlen <= strlen(pathname)) {
        pathlen = strlen(pathname) * 2;
        if ((fullpath = realloc(fullpath, pathlen)) == NULL)
            err_sys("realloc failed");
    }

    strcpy(fullpath, pathname);
    return dopath(func);
}


static int
dopath(Myfunc *func)
{
    struct stat statbuf;
    struct dirent *dirp;
    DIR *dp;
    int ret, n;

    if (lstat(fullpath, &statbuf) < 0)
        return func(fullpath, &statbuf, FTW_NS);
    if (S_ISDIR(statbuf.st_mode) == 0)
        return func(fullpath, &statbuf, FTW_F);

    if ((ret = func(fullpath, &statbuf, FTW_D)) != 0)
        return ret;
    n = strlen(fullpath);
    if (n + NAME_MAX + 2 > pathlen) {
        pathlen *= 2;
        if ((fullpath = realloc(fullpath, pathlen)) == 0)
            err_sys("realloc failed");
    }

    fullpath[n++] = '/';
    fullpath[n] = 0;
    if ((dp = opendir(fullpath)) == NULL)
        return func(fullpath, &statbuf, FTW_DNR);

    while ((dirp = readdir(dp)) != NULL) {
        if (strcmp(dirp->d_name, ".") == 0 ||
            strcmp(dirp->d_name, "..") == 0)
            continue;
        strcpy(&fullpath[n], dirp->d_name);
        if ((ret = dopath(func)) != 0)
            break;
    }

    fullpath[n-1] = 0;
    if (closedir(dp) < 0)
        err_ret("can not close directory %s", fullpath);

    return ret;
}

static int
myfunc(const char *pathname, const struct stat *statptr, int type)
{
    switch(type) {
    case FTW_F:
        switch (statptr->st_mode & S_IFMT) {
        case S_IFREG:
            printf("%s: file\n", pathname);
            break;
        case S_IFBLK:
            printf("%s: block\n", pathname);
            break;
        case S_IFCHR:
            printf("%s: char\n", pathname);
            break;
        case S_IFLNK:
            printf("%s: link\n", pathname);
            break;
        default:
            break;
        }
        break;
    case FTW_D:
        printf("%s: directory\n", pathname);
        break;
    case FTW_DNR:
        err_ret("%s: read directory error\n", pathname);
        break;
    case FTW_NS:
        err_ret("%s: stat error\n", pathname);
        break;
    default:
        err_dump("%s: unknown type\n", pathname);
    }

    return 0;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
        err_quit("usage: ftw <starting-pathname>");
    return myftw(argv[1], myfunc);
}

