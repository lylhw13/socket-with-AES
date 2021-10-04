#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include "encrypt.h"

#define SA struct sockaddr

unsigned char *key  = (unsigned char *)"01234567890123456789012345678901";
unsigned char *iv = (unsigned char *)"0123456789012345";
unsigned char *plaintext = (unsigned char *)"The quick brown fox jumps over the lazy dog";
int ciphertext_len;

ssize_t rio_writen(int fd, void *buf, size_t n)
{
    size_t nleft = n;
    ssize_t nwrite;
    char *bufp = buf;

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

int main(int argc, char *argv[])
{
    int listenfd, connfd;
    int port = 33333;
    socklen_t len;
    int n;
    unsigned char buf[BUFSIZE];
    unsigned char ciphertext[BUFSIZE];

    int nread, nwrite;

    struct sockaddr_in servaddr, cliaddr;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

    listen(listenfd, 64);

    len = sizeof(cliaddr);
    connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &len);

    ctx_init(key, iv);
    int i = 0;
    while ((nread = read(STDIN_FILENO, buf, BUFSIZE - 1)) > 0) {
    // ctx_init(key, iv);
        i++;

        ctx_reset(key, iv);
        printf("i %d, nread %d, ", i, nread);

        ciphertext_len = encry(buf, nread, ciphertext);
        // nwrite = write(connfd, ciphertext, ciphertext_len);
        nwrite = rio_writen(connfd, ciphertext, ciphertext_len);
        // if (nwrite < nread)
        //     printf("nread %d, nwrite %d\n", nread, nwrite);

        printf("nwrite %d\n", nwrite);
        // printf("ciphertext %d, nwrite %d\n", ciphertext_len, nwrite);
        // BIO_dump_fp(stdout, ciphertext, ciphertext_len);
        // printf("\n");
        // BIO_dump_fp(stdout, buf, nread);

        // ctx_cleanup();
    }

    ctx_cleanup();
    return 0;
}