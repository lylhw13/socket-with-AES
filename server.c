#include "encrypt.h"
#include "generic.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>


unsigned char *key  = (unsigned char *)"01234567890123456789012345678901";
unsigned char *iv = (unsigned char *)"0123456789012345";


int main(int argc, char *argv[])
{
    int listenfd, connfd;
    int port = 33333;
    socklen_t clilen;
    int n, nread;
    unsigned char buf[BUFSIZE];
    unsigned char ciphertext[BUFSIZE];
    int ciphertext_len;

    struct sockaddr_in servaddr, cliaddr;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd < 0) 
        error("socket");
    
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
        error("bind");
    
    if (listen(listenfd, 64) < 0)
        error("listen");

    clilen = sizeof(cliaddr);
    connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen);
    if (connfd < 0)
        error("accept");

    ctx_init(key, iv);
    while ((nread = read(STDIN_FILENO, buf, BUFSIZE - 1)) > 0) {
        ctx_reset(key, iv);
        ciphertext_len = encry(buf, nread, ciphertext);
        if (rio_writen(connfd, ciphertext, ciphertext_len) < 0)
            error("rio_writen");
    }

    ctx_cleanup();

    return 0;
}