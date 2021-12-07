#include <stdint.h>

#ifndef AES_SELF_AES128_BLCOK_CRYPT_H
#define AES_SELF_AES128_BLCOK_CRYPT_H

// 秘密扩展
void key_expansion(uint8_t key_text[16]);

// 对块进行加密
void block_encrypt(uint8_t message_text[16], uint8_t state_matrix[4][4]);

// 对块进行解密
void block_decrypt(uint8_t message_text[16], uint8_t state_matrix[4][4]);

#endif //AES_SELF_AES128_BLCOK_CRYPT_H
