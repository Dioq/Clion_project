#include "aes128_crypt.h"

// 矩阵 转 文本
void matrix_to_text(uint8_t text[16], uint8_t origin_matrix[4][4]) {
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            text[4 * i + j] = origin_matrix[j][i];
        }
    }
}

void print_matrix(uint8_t target[4][4]) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            printf("%02X ", target[i][j]);
        }
        puts("");
    }
    puts("");
}

/*void split_block(Message *msg) {
    // 把数据长度加在数据头部 需占用 8 Byte
    unsigned long add_8byte_len = msg->len + 8;
    unsigned long block_num = add_8byte_len / 16;
    uint8_t remainder = add_8byte_len % 16;
    if (remainder != 0) {
        block_num++;
    }
    unsigned long new_len = block_num * 16;
    uint8_t *new_p = (uint8_t *) calloc(new_len, 1);
    if (!new_p) {
        printf("Not Enough Memory!\n");
        exit(-1);
    }
    unsigned long num = msg->len;
    uint8_t *data_len_ptr = (uint8_t *) &num;
    memcpy(new_p, data_len_ptr, 8);
    memcpy(new_p + 8, msg->message, msg->len);
    if (remainder != 0) {
        // 尾部填充 0,最后一字节是填充的个数
        uint8_t filling_num = 16 - remainder; // new_len - (msg->len + 8);
        memset((new_p + 8 + msg->len), '\0', filling_num - 1);
        *(new_p + new_len - 1) = filling_num;
    }
    msg->message = new_p;
    msg->len = new_len;
*//*    printf("split_block len =  0x%lx:\n", msg->len);
    for (int i = 0; i < msg->len; ++i) {
        printf("0x%02X ", *(msg->message + i));
    }
    puts("");*//*
}

// 处理填充 还原文本真实长度
void handle_filling(Message *msg) {
    msg->len = (unsigned long) *(msg->message);
    msg->message += 8;
}*/


void split_block(Message *msg) {
    unsigned long block_num = msg->len / 16;
    uint8_t remainder = msg->len % 16;
//    printf("block_num = %lu\t remainder = %u\n", block_num, remainder);
    if (remainder != 0) {
        block_num++;
        unsigned long new_len = block_num * 16;
        uint8_t *new_p = (uint8_t *) realloc(msg->message, new_len);
        if (!new_p) {
            printf("Not Enough Memory!\n");
            exit(-1);
        }
        // 尾部填充 0， 最后一字节是填充个数
        uint8_t filling_num = 16 - remainder;
        memset((new_p + new_len - filling_num), '\0', filling_num);
        *(new_p + new_len - 1) = filling_num;
        msg->message = new_p;
        msg->len = new_len;
    }
}

// 判断是否有填充，以及填充个数
void handle_filling(Message *msg) {
    uint8_t *ptr = msg->message + msg->len - 1;
    uint8_t filling_num = *ptr;
    // 确定填充的真实性
    uint8_t is_filling = 0;
    if (filling_num != 0) {
        for (int i = 1; i < filling_num; ++i) {
            if (*(ptr - i) != '\0') {
                is_filling++;
            }
        }
    }
    // 修正真实数据个数
    if (is_filling == 0) {
        msg->len -= filling_num;
    }
}

void AES_ECB_encrypt(uint8_t key[16], Message *msg) {
    // 分隔成 块，不足16 字节 进行填充
    split_block(msg);

    uint8_t *ptr = msg->message;
    key_expansion(key);
    uint8_t state_matrix[4][4];
    for (int i = 0; i < msg->len / 16; ++i) {
        block_encrypt(ptr, state_matrix);
/*        puts("state_matrix:");
        print_matrix(state_matrix);
        puts("");*/
        matrix_to_text(ptr, state_matrix);
        ptr += 16;
    }
}

void AES_ECB_decrypt(uint8_t key[16], Message *msg) {
    uint8_t *ptr = msg->message;
    key_expansion(key);
    uint8_t state_matrix[4][4];
    for (int i = 0; i < msg->len / 16; ++i) {
        block_decrypt(ptr, state_matrix);
        matrix_to_text(ptr, state_matrix);
        ptr += 16;
    }
    handle_filling(msg);
}

uint8_t iv[16] = "abcdef0123456789";

// 两个数组里的数据依次 异或
void xor_arr(uint8_t msg[16], const uint8_t object[16]) {
    for (int i = 0; i < 16; ++i) {
        msg[i] = msg[i] ^ object[i];
    }
}

void AES_CBC_encrypt(uint8_t key[16], Message *msg) {
    // 分隔成 块，不足16 字节 进行填充
    split_block(msg);

    uint8_t *ptr = msg->message;
    key_expansion(key);
    uint8_t state_matrix[4][4];

    // 需要每次与明文异或的 数据
    uint8_t *obj_ptr = iv;
    for (int i = 0; i < msg->len / 16; ++i) {
        xor_arr(ptr, obj_ptr);
        block_encrypt(ptr, state_matrix);
/*        puts("state_matrix:");
        print_matrix(state_matrix);
        puts("");*/
        matrix_to_text(ptr, state_matrix);
        obj_ptr = ptr;
        ptr += 16;
    }
}

void AES_CBC_decrypt(uint8_t key[16], Message *msg) {
    uint8_t *ptr = msg->message;
    key_expansion(key);
    uint8_t state_matrix[4][4];

    // 需要每次与明文异或的 数据
    uint8_t *obj_ptr = (uint8_t *) calloc(msg->len, 1);
    memcpy(obj_ptr, iv, 16);
    memcpy(obj_ptr + 16, msg->message, (msg->len - 16));
    for (int i = 0; i < msg->len / 16; ++i) {
        block_decrypt(ptr, state_matrix);
        matrix_to_text(ptr, state_matrix);
        xor_arr(ptr, obj_ptr);
        ptr += 16;
        obj_ptr += 16;
    }
    handle_filling(msg);
}
