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

void split_block(Message *msg) {
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
/*    printf("split_block len =  0x%lx:\n", msg->len);
    for (int i = 0; i < msg->len; ++i) {
        printf("0x%02X ", *(msg->message + i));
    }
    puts("");*/
}

// 处理填充 还原文本真实长度
void handle_filling(Message *msg) {
    msg->len = (unsigned long) *(msg->message);
    msg->message += 8;
}

// iv
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
        matrix_to_text(ptr, state_matrix);
        obj_ptr = ptr;
        ptr += 16;
    }
    obj_ptr = NULL;
    ptr = NULL;
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
