#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include "encrypt.h"

#define SA struct sockaddr

unsigned char *key  = (unsigned char *)"01234567890123456789012345678901";
unsigned char *iv = (unsigned char *)"0123456789012345";

ssize_t rio_readn(int fd, void *buf, size_t n)
{
    size_t nleft = n;
    ssize_t nread;
    char *bufp = buf;
    while (nleft > 0) {
        if ((nread = read(fd, buf, nleft)) < 0) {
            if (errno == EINTR)
                nread = 0;
            else
                return -1;
        }
        else if (nread == 0)
            break;

        nleft -= nread;
        bufp += nread;
    }

    return (n - nleft);
}

int main(int argc, char *argv)
{
    char *addr = "127.0.0.1";
    int port = 33333;
    int sockfd, n;
    struct sockaddr_in sockaddr;
    unsigned char buf[BUFSIZE];    
    unsigned char decryptedtext[BUFSIZE];
    int decryptedtext_len, nread, nwrite;
    unsigned long int errorcode;
    int offset = 0;

    
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        perror("socket");

    memset(&sockaddr, 0, sizeof(sockaddr));
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_port = htons(port);
    inet_pton(AF_INET, addr, &sockaddr.sin_addr);

    if (connect(sockfd, (SA *)&sockaddr, sizeof(sockaddr)) < 0)
        perror("connect");

    ctx_init(key, iv);
    while ((nread = read(sockfd, buf + offset, BUFSIZE - offset)) > 0) {


        ctx_reset(key, iv);
 
        // BIO_dump_fp(stdout, buf, nread);
        // if (nread < BUFSIZE)
        //     fprintf(stderr, "i %d, nread %d ",i, nread);

        // tmp = (nread + offset) % 16;

        // decryptedtext_len = decry(buf, nread, decryptedtext);
        decryptedtext_len = decry(buf, nread + offset, decryptedtext);


        if (decryptedtext_len < 0) {
            errorcode = ERR_peek_last_error();
            if (errorcode == 0x0606506D || errorcode == 0x06065064) {
                fprintf(stderr, "errrocode is %08lx\n", errorcode);
                offset = nread;
                continue;
                // fprintf(stderr,"\n");
            }
            else {
                ERR_print_errors_fp(stderr);
                abort();
            }
        }
        else {
            offset = 0;
        }

        // offset = nread + offset - decryptedtext_len;
        // offset = tmp;

        // memmove(buf, buf + decryptedtext_len, offset);

        if ((nwrite = write(STDOUT_FILENO, decryptedtext, decryptedtext_len)) < 0)
            perror("write");

        // if (nread < BUFSIZE)
        //     fprintf(stderr, ", nwrite %d\n", nwrite);
        // // fprintf(stderr, "nread %d, nwrite %d, decry_len %d\n", nread, nwrite, decryptedtext_len);
        // BIO_dump_fp(stderr, buf, nread);
        // fprintf(stderr, "\n");
        // BIO_dump_fp(stderr, decryptedtext, decryptedtext_len);
    // ctx_cleanup();
    }

    // while ((nread = rio_readn(sockfd, buf, BUFSIZE)) > 0) {
    //     ctx_reset(key, iv);
    //     i++;
 
    //     fprintf(stderr, "i %d, nread %d ",i, nread);

    //     decryptedtext_len = decry(buf, nread, decryptedtext);

    //     if ((nwrite = write(STDOUT_FILENO, decryptedtext, decryptedtext_len)) < 0)
    //         perror("write");

    //     fprintf(stderr, ", nwrite %d\n", nwrite);
    // }


    if (n < 0)
        perror("read");

    ctx_cleanup();
    return 0;
}