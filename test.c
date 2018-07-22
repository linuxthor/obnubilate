#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include "aes.h"

#define CBC 1
static void test_encrypt_cbc(unsigned char *str);

int main(int ac, char **av)
{
    FILE *fp = NULL;
    unsigned char str[16];

    if (ac < 3)
    {
        printf("[%s] <string> <file>\n",av[0]);
        exit(1);
    }

    if (strlen(av[1]) != 16)
    {
        printf("String must be exactly 16 characters!\n");
        exit(1);
    }
    memcpy(str,av[1],16);
    test_encrypt_cbc(str);

    fp = fopen( av[2] , "wb+" );
    fwrite(str, 1, sizeof(str), fp );
    fclose(fp);

    printf("Done\n");
    return 0;
}

static void test_encrypt_cbc(unsigned char *str)
{
    uint8_t key[] = { 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41,
                      0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41 };
    uint8_t iv[]  = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

    struct AES_ctx ctx;
    AES_init_ctx_iv(&ctx, key, iv);
    AES_CBC_encrypt_buffer(&ctx, str, 16);
}
