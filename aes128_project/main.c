#include <stdio.h>
#include <string.h>
#include "aes128_crypt.h"

void test_aes128_EBC();

void test_aes128_CBC();

int main() {
    test_aes128_CBC();
    puts("\n*******************************");
    test_aes128_EBC();
    return 0;
}

void test_aes128_CBC() {
//    uint8_t key_text[16] = {0x3C, 0xA1, 0x0B, 0x21, 0x57, 0xF0, 0x19, 0x16, 0x90, 0x2E, 0x13, 0x80, 0xAC, 0xC1, 0x07,
//                            0xBD};
//    uint8_t key_text[16] = {'a', 'a', 'a', 'a', 'b', 'b', 'b', 'b', 'c', 'c', 'c',
//                            'c', 'd', 'd', 'd', 'd'};
    uint8_t key[16] = "aaaabbbbccccdddd";
    char *text = "Hello world!Be successHope to be successJust do it.Do it!";
    unsigned long len = strlen(text);
    printf("origin message len = %lu\n", len);
/*    uint8_t text[] = {0x48, 0x65, 0x6C, 0x6C, 0x6F, 0x20, 0x77, 0x6F, 0x72, 0x6C, 0x64, 0x21, 0x0A, 0x42,
                      0x65, 0x20, 0x73, 0x75, 0x63, 0x63, 0x65, 0x73, 0x73, 0x0A, 0x48, 0x6F, 0x70, 0x65,
                      0x20, 0x74, 0x6F, 0x20, 0x62, 0x65, 0x20, 0x73, 0x75, 0x63, 0x63, 0x65, 0x73, 0x73,
                      0x0A, 0x4A, 0x75, 0x73, 0x74, 0x20, 0x64, 0x6F, 0x20, 0x69, 0x74, 0x2E, 0x0A, 0x44,
                      0x6F, 0x20, 0x69, 0x74, 0x21, 0x0A};
    unsigned long len = sizeof(text) / sizeof(uint8_t);*/
    uint8_t *plaintext_text = (uint8_t *) malloc(len);
    strcpy(plaintext_text, text);
    Message *msg = (Message *) malloc(sizeof(Message));
    msg->message = plaintext_text;
    msg->len = len;
    AES_CBC_encrypt(key, msg);
    printf("len = %lu ciphertext:\n", msg->len);
    for (int i = 0; i < msg->len; ++i) {
        printf("0x%02X ", *(msg->message + i));
    }
    puts("");

    AES_CBC_decrypt(key, msg);
    printf("len = %lu plaintext:\n", msg->len);
    for (int i = 0; i < msg->len; ++i) {
        printf("0x%02X ", *(msg->message + i));
    }
    puts("");
    for (int i = 0; i < msg->len; ++i) {
        printf("%c", *(msg->message + i));
    }
}

void test_aes128_EBC() {
    uint8_t key_text[16] = {0x3C, 0xA1, 0x0B, 0x21, 0x57, 0xF0, 0x19, 0x16, 0x90, 0x2E, 0x13, 0x80, 0xAC, 0xC1, 0x07,
                            0xBD};
    char *text = "Hello world!Be successHope to be successJust do it.Do it!";
    unsigned long len = strlen(text);
    printf("origin message len = %lu\n", len);
    uint8_t *plaintext_text = (uint8_t *) malloc(len);
    strcpy(plaintext_text, text);
    Message *msg = (Message *) malloc(sizeof(Message));
    msg->message = plaintext_text;
    msg->len = len;
    AES_ECB_encrypt(key_text, msg);
    printf("len = %lu ciphertext:\n", msg->len);
    for (int i = 0; i < msg->len; ++i) {
        printf("0x%02X ", *(msg->message + i));
    }
    puts("");

    AES_ECB_decrypt(key_text, msg);
    printf("len = %lu plaintext:\n", msg->len);
    for (int i = 0; i < msg->len; ++i) {
        printf("%c", *(msg->message + i));
    }
}
