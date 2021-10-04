#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

#define SA struct sockaddr

static void error(const char *str)
{
    perror(str);
    exit(EXIT_FAILURE);
}

ssize_t rio_writen(int fd, void *buf, size_t n)
{
    size_t nleft = n;
    ssize_t nwrite;
    char *bufp = (char *)buf;

    while (nleft > 0) {
        if ((nwrite = write(fd, buf, nleft)) <= 0) {
            if (errno == EINTR) 
                nwrite = 0;
            else
                return -1;
        }

        nleft -= nwrite;
        bufp += nwrite;
    }
    return n;
}