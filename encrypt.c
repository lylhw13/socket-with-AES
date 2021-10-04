#include "encrypt.h"
#include <unistd.h>

static EVP_CIPHER_CTX *d_ctx;
static EVP_CIPHER_CTX *e_ctx;

void handleErrors(void)
{
    ERR_print_errors_fp(stderr);
    abort();
}

static void _enc_ctx_init(EVP_CIPHER_CTX **ctx, unsigned char *key, unsigned char *iv, int enc)
{
    if(!(*ctx = EVP_CIPHER_CTX_new()))
        handleErrors();

    if (1 != EVP_CipherInit_ex(*ctx, EVP_aes_256_cbc(), NULL, key, iv, enc))
        handleErrors();
}

void ctx_init(unsigned char *key, unsigned char *iv)
{
    _enc_ctx_init(&e_ctx, key, iv, 1);    /* 1 for encryption */
    _enc_ctx_init(&d_ctx, key, iv, 0);    /* 0 for decryption */
}

void _enc_ctx_reset(EVP_CIPHER_CTX **ctx, unsigned char *key, unsigned char *iv, int enc)
{
    if (1 != EVP_CipherInit_ex(*ctx, EVP_aes_256_cbc(), NULL, key, iv, enc))
        handleErrors();
}

void ctx_reset(unsigned char *key, unsigned char *iv)
{
    EVP_CIPHER_CTX_reset(e_ctx);
    EVP_CIPHER_CTX_reset(d_ctx);
    _enc_ctx_init(&e_ctx, key, iv, 1);    /* 1 for encryption */
    _enc_ctx_init(&d_ctx, key, iv, 0);    /* 0 for decryption */
}


static int encry_decry_ctx(unsigned char *inbuf, int in_len, EVP_CIPHER_CTX *ctx, unsigned char *outbuf)
{
    int len, out_len;

    // if (ctx != NULL) {
    //     // unsigned char mybuf[BUFSIZE];

    //     if (1 != EVP_CipherUpdate(ctx, outbuf, &len, (unsigned char *)inbuf, in_len))
    //         handleErrors();
    //     out_len = len;

    //     if (1 != EVP_CipherFinal_ex(ctx, outbuf + len, &len))
    //         handleErrors();
    //     out_len += len;
    //     // memcpy(outbuf, mybuf, in_len);
    // }
    // return out_len;

/* work for big file */
    if (ctx != NULL) {
        unsigned char mybuf[BUFSIZE];

        if (1 != EVP_CipherUpdate(ctx, mybuf, &len, (unsigned char *)inbuf, in_len)) {
            // ERR_print_errors_fp(stderr);
            // printf("update error code is %ld\n",ERR_get_error());
            return -1;

            // handleErrors();
        }
        out_len = len;

        if (1 != EVP_CipherFinal_ex(ctx, mybuf + len, &len)) {
            // handleErrors();
            // printf("finale error code is %08lx %08lx\n", ERR_get_error(),ERR_peek_last_error());
            // ERR_print_errors_fp(stderr);
            return -1;
        }

        out_len += len;
        memcpy(outbuf, mybuf, out_len);
    }
    return out_len;

    // if (ctx != NULL) {
    //     unsigned char mybuf[BUFSIZE];

    //     if (1 != EVP_CipherUpdate(ctx, mybuf, &len, (unsigned char *)inbuf, in_len))
    //         handleErrors();

    //     // if (1 != EVP_CipherFinal_ex(ctx, mybuf + len, &len))
    //     //     handleErrors();
    //     // out_len += len;
    //     memcpy(outbuf, mybuf, in_len);
    // }
    // return in_len;

    // if (ctx != NULL) {
    //     unsigned char mybuf[BUFSIZE];

    //     if (1 != EVP_CipherUpdate(ctx, mybuf, &len, (unsigned char *)inbuf, in_len))
    //         handleErrors();
    //     out_len = len;

    //     // if (1 != EVP_CipherFinal_ex(ctx, mybuf + len, &len))
    //     //     handleErrors();
    //     // out_len += len;
    //     memcpy(outbuf, mybuf, out_len);
    // }
    // return out_len;
}

int encry(unsigned char *plaintext, int plaintext_len, unsigned char *ciphertext)
{
    // char buf[BUFSIZE];
    // int len;
    // len = encry_decry_ctx(plaintext, plaintext_len, e_ctx, buf);
    // memcpy(ciphertext, buf, len);
    // return len;
    return encry_decry_ctx(plaintext, plaintext_len, e_ctx, ciphertext);
}

int decry(unsigned char *ciphertext, int ciphertext_len, unsigned char *plaintext)
{
    // char buf[BUFSIZE];
    // int len;
    // len = encry_decry_ctx(ciphertext, ciphertext_len, d_ctx, buf);
    // memcpy(plaintext, buf, len);
    // return len;
    return encry_decry_ctx(ciphertext, ciphertext_len, d_ctx, plaintext);
}



void ctx_cleanup(void)
{
    EVP_CIPHER_CTX_free(e_ctx);
    EVP_CIPHER_CTX_free(d_ctx);
}

#define KEY_LEN 32
#define IV_LEN 16

// #define BUFSIZE 16

// int main()
// {
//     /* A 256 bit key */
//     // unsigned char *key = (unsigned char *)"6ccf2517b89369695182fc1d0581662c";

//     // /* A 128 bit IV */
//     // unsigned char *iv = (unsigned char *)"b5fb2c86683cfe3e";
//     unsigned char *key  = (unsigned char *)"01234567890123456789012345678901";
//     // unsigned char *key2 = (unsigned char *)"01234567890123456789012345111111";

//     /* A 128 bit IV */
//     unsigned char *iv = (unsigned char *)"0123456789012345";

//     // unsigned char buf[BUFSIZE];
//     // unsigned char ciphertext[BUFSIZE];
//     // unsigned char decryptedtext[BUFSIZE];
//     int nread, nwrite;

//     // ctx_init(key,iv);

//     // while ((nread = read(STDIN_FILENO, buf, BUFSIZE - 1)) > 0) {
//     // ctx_init(key,iv);

//     //     // printf("\nnread is %d\n", nread);
//     //     // ctx_reset(key, iv);
//     //     int decryptedtext_len, ciphertext_len;

//     //     ciphertext_len = encry(buf, nread, ciphertext);

//     //     // fprintf(stderr, "\nread is :\n");
//     //     // write(STDERR_FILENO, buf, nread);
//     //     // fprintf(stderr, "\nCiphertext is:\n");
//     //     // BIO_dump_fp (stderr, (const char *)ciphertext, ciphertext_len);

//     //     decryptedtext_len = decry(ciphertext, ciphertext_len - 5, decryptedtext);

//     //     // printf("\nDecry is:\n");
//     //     // BIO_dump_fp(stderr, (const char *)decryptedtext, decryptedtext_len);

//     //     write(STDOUT_FILENO, decryptedtext, decryptedtext_len);
//     //     ctx_cleanup();
//     // }

//     // ctx_cleanup();
 

//     // enc_ctx_init(key2, iv);
//     // cleanup();

//     ctx_init(key,iv);

//     unsigned char *plaintext = (unsigned char *)"The quick brown fox jumps over the lazy dog";

//     unsigned char ciphertext[128];
//     unsigned char decryptedtext[128];
//     unsigned long int errorcode = 0;

//     int decryptedtext_len, ciphertext_len;
    

//     printf("before\n");
//     if (e_ctx == NULL){
//         puts("e_ctx is NULL");
//     }
//     ciphertext_len = encry(plaintext, strlen((char *)plaintext), ciphertext);
//     // ciphertext_len = encry(plaintext, strlen((char *)plaintext), ciphertext);


//     printf("Ciphertext is:\n");
//     BIO_dump_fp (stdout, (const char *)ciphertext, ciphertext_len);


//     decryptedtext_len = decry(ciphertext, ciphertext_len - 16, decryptedtext);
// /* error:06065064:digital envelope routines:EVP_DecryptFinal_ex:bad decrypt:../crypto/evp/evp_enc.c:610: */
// /* error:0606506D:digital envelope routines:EVP_DecryptFinal_ex:wrong final block length:../crypto/evp/evp_enc.c:599: */
//     if (decryptedtext_len < 0) {
//         errorcode = ERR_peek_last_error();
//         if (errorcode == 0x0606506D || errorcode == 0x06065064) {
//             fprintf(stderr, "errrocode is %08lx\n", errorcode);
//             ERR_print_errors_fp(stderr);
//         }
//     }

//     printf("decrypedtext_len %d \n", decryptedtext_len);

//     // printf("\nDecry is:\n");
//     // BIO_dump_fp(stdout, (const char *)decryptedtext, decryptedtext_len);

//     // /* Add a NULL terminator. We are expecting printable text */
//     decryptedtext[decryptedtext_len] = '\0';

//     // /* Show the decrypted text */
//     printf("Decrypted text is:\n");
//     printf("%s\n", decryptedtext);


//     ctx_cleanup();

//     return 0;
// }