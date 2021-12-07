#include "aes128_block_crypt.h"
#include "aes_struct.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifndef AES128_AES128_CRYPT_H
#define AES128_AES128_CRYPT_H

void AES_CBC_encrypt(uint8_t key[16], Message *msg);

void AES_CBC_decrypt(uint8_t key[16], Message *msg);

#endif //AES128_AES128_CRYPT_H
