#include "encrypt.h"
#include "generic.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>


unsigned char *key  = (unsigned char *)"01234567890123456789012345678901";
unsigned char *iv = (unsigned char *)"0123456789012345";


int main(int argc, char *argv)
{
    char *addr = "127.0.0.1";
    int port = 33333;
    int sockfd;
    struct sockaddr_in sockaddr;
    unsigned char buf[BUFSIZE];    
    unsigned char decryptedtext[BUFSIZE];
    int decryptedtext_len, nread, offset = 0;
    unsigned long int errorcode;
    
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        error("socket");

    memset(&sockaddr, 0, sizeof(sockaddr));
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_port = htons(port);
    inet_pton(AF_INET, addr, &sockaddr.sin_addr);

    if (connect(sockfd, (SA *)&sockaddr, sizeof(sockaddr)) < 0)
        error("connect");

    ctx_init(key, iv);

    while ((nread = read(sockfd, buf + offset, BUFSIZE - offset)) > 0) {

        ctx_reset(key, iv);

        decryptedtext_len = decry(buf, nread + offset, decryptedtext);
        if (decryptedtext_len < 0) {
            errorcode = ERR_peek_last_error();
            if (errorcode == 0x0606506D || errorcode == 0x06065064) {
                // fprintf(stderr, "errrocode is %08lx\n", errorcode);
                offset = nread;
                continue;
            }
            else {
                ERR_print_errors_fp(stderr);
                abort();
            }
        }
        else {
            offset = 0;
        }

        if (rio_writen(STDOUT_FILENO, decryptedtext, decryptedtext_len) < 0)
            error("rio_writen");
    }

    ctx_cleanup();

    return 0;
}