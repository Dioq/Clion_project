#include "aes128_block_crypt.h"

// S-盒
uint8_t sbox[16][16] =
        {
                {0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76},
                {0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0},
                {0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15},
                {0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75},
                {0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84},
                {0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf},
                {0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8},
                {0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2},
                {0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73},
                {0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb},
                {0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79},
                {0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08},
                {0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a},
                {0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e},
                {0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf},
                {0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16}
        };
// 逆 S-盒
uint8_t inv_sbox[16][16] =
        {
                {0x52, 0x09, 0x6A, 0xD5, 0x30, 0x36, 0xA5, 0x38, 0xBF, 0x40, 0xA3, 0x9E, 0x81, 0xF3, 0xD7, 0xFB},
                {0x7C, 0xE3, 0x39, 0x82, 0x9B, 0x2F, 0xFF, 0x87, 0x34, 0x8E, 0x43, 0x44, 0xC4, 0xDE, 0xE9, 0xCB},
                {0x54, 0x7B, 0x94, 0x32, 0xA6, 0xC2, 0x23, 0x3D, 0xEE, 0x4C, 0x95, 0x0B, 0x42, 0xFA, 0xC3, 0x4E},
                {0x08, 0x2E, 0xA1, 0x66, 0x28, 0xD9, 0x24, 0xB2, 0x76, 0x5B, 0xA2, 0x49, 0x6D, 0x8B, 0xD1, 0x25},
                {0x72, 0xF8, 0xF6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xD4, 0xA4, 0x5C, 0xCC, 0x5D, 0x65, 0xB6, 0x92},
                {0x6C, 0x70, 0x48, 0x50, 0xFD, 0xED, 0xB9, 0xDA, 0x5E, 0x15, 0x46, 0x57, 0xA7, 0x8D, 0x9D, 0x84},
                {0x90, 0xD8, 0xAB, 0x00, 0x8C, 0xBC, 0xD3, 0x0A, 0xF7, 0xE4, 0x58, 0x05, 0xB8, 0xB3, 0x45, 0x06},
                {0xD0, 0x2C, 0x1E, 0x8F, 0xCA, 0x3F, 0x0F, 0x02, 0xC1, 0xAF, 0xBD, 0x03, 0x01, 0x13, 0x8A, 0x6B},
                {0x3A, 0x91, 0x11, 0x41, 0x4F, 0x67, 0xDC, 0xEA, 0x97, 0xF2, 0xCF, 0xCE, 0xF0, 0xB4, 0xE6, 0x73},
                {0x96, 0xAC, 0x74, 0x22, 0xE7, 0xAD, 0x35, 0x85, 0xE2, 0xF9, 0x37, 0xE8, 0x1C, 0x75, 0xDF, 0x6E},
                {0x47, 0xF1, 0x1A, 0x71, 0x1D, 0x29, 0xC5, 0x89, 0x6F, 0xB7, 0x62, 0x0E, 0xAA, 0x18, 0xBE, 0x1B},
                {0xFC, 0x56, 0x3E, 0x4B, 0xC6, 0xD2, 0x79, 0x20, 0x9A, 0xDB, 0xC0, 0xFE, 0x78, 0xCD, 0x5A, 0xF4},
                {0x1F, 0xDD, 0xA8, 0x33, 0x88, 0x07, 0xC7, 0x31, 0xB1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xEC, 0x5F},
                {0x60, 0x51, 0x7F, 0xA9, 0x19, 0xB5, 0x4A, 0x0D, 0x2D, 0xE5, 0x7A, 0x9F, 0x93, 0xC9, 0x9C, 0xEF},
                {0xA0, 0xE0, 0x3B, 0x4D, 0xAE, 0x2A, 0xF5, 0xB0, 0xC8, 0xEB, 0xBB, 0x3C, 0x83, 0x53, 0x99, 0x61},
                {0x17, 0x2B, 0x04, 0x7E, 0xBA, 0x77, 0xD6, 0x26, 0xE1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0C, 0x7D}
        };

// 单位矩阵，用于列混合
const uint8_t M[4][4] = {
        {0x02, 0x03, 0x01, 0x01},
        {0x01, 0x02, 0x03, 0x01},
        {0x01, 0x01, 0x02, 0x03},
        {0x03, 0x01, 0x01, 0x02}
};
// 单位矩阵，用于列混合还原
const uint8_t inv_M[4][4] = {
        {0x0E, 0x0B, 0x0D, 0x09},
        {0x09, 0x0E, 0x0B, 0x0D},
        {0x0D, 0x09, 0x0E, 0x0B},
        {0x0B, 0x0D, 0x09, 0x0E}
};
// 生成轮秘钥时的常数
uint8_t rcon[10][4] = {
        {0x01, 0x00, 0x00, 0x00},
        {0x02, 0x00, 0x00, 0x00},
        {0x04, 0x00, 0x00, 0x00},
        {0x08, 0x00, 0x00, 0x00},
        {0x10, 0x00, 0x00, 0x00},
        {0x20, 0x00, 0x00, 0x00},
        {0x40, 0x00, 0x00, 0x00},
        {0x80, 0x00, 0x00, 0x00},
        {0x1B, 0x00, 0x00, 0x00},
        {0x36, 0x00, 0x00, 0x00}
};

// 将输入文本转化成 需要的矩阵
void text_to_matrix(uint8_t text[16], uint8_t target_matrix[4][4]) {
    for (int k = 0; k < 16; k++) {
        int i = k % 4;
        int j = k / 4;
        target_matrix[i][j] = text[k];
    }
}

// 行左移 steps 位
void left_move(uint8_t row[4], int steps) {
    int len = 4;
    uint8_t new_arr[4];
    for (int i = 0; i < len; i++) {
        int index = (steps + i) % len;
        new_arr[i] = row[index];
    }
    for (int i = 0; i < len; i++) {
        row[i] = new_arr[i];
    }
}

// 行右移 steps 位
void right_move(uint8_t row[4], int steps) {
    int len = 4;
    uint8_t new_arr[4];
    for (int i = 0; i < len; i++) {
        int index = (4 - steps + i) % len;
        new_arr[i] = row[index];
    }
    for (int i = 0; i < len; i++) {
        row[i] = new_arr[i];
    }
}

// GF(2^8) 乘法
uint8_t GMul(uint8_t u, uint8_t v) {
    uint8_t p = 0;
    for (int i = 0; i < 8; ++i) {
        if (u & 0x01) {
            p ^= v;
        }
        int flag = (v & 0x80);
        v <<= 1;
        if (flag) {
            v ^= 0x1B;
        }
        u >>= 1;
    }
    return p;
}

// S 盒 替换 对象是一个数组
void sbox_replace_arr(uint8_t target[4]) {
    for (int j = 0; j < 4; j++) {
        uint8_t element = target[j];
        //字节的高4位作为行值，低4位作为列值
        uint8_t row = (element & 0xF0) >> 4;
        uint8_t column = element & 0x0F;
        //取出S盒对应的元素
        uint8_t new_element = sbox[row][column];
        //替换原矩阵中的元素
        target[j] = new_element;
    }
}


// 扩展秘钥，以2维数组的的形式存放.
uint8_t W[44][4];

// 秘密扩展，将传过来的秘钥扩展 成 11 组
void key_expansion(uint8_t key_text[16]) {
    uint8_t key_matrix[4][4];
    text_to_matrix(key_text, key_matrix);
//    print_matrix(key_matrix);

    // 生成 W[0] - W[3]
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            W[i][j] = key_matrix[j][i];
        }
    }

    // 生成 W[4] - W[43]
    for (int i = 4; i < 44; ++i) {
        /*
            1. 如果i不是4的倍数，那么第i列由如下等式确定：
                W[i]=W[i-4]⨁W[i-1]
            2. 如果i是4的倍数，那么第i列由如下等式确定：
                W[i]=W[i-4]⨁T(W[i-1])
                其中，T是一个有点复杂的函数。
                函数T由3部分组成：字循环、字节代换和轮常量异或，这3部分的作用分别如下。
                a.字循环：将1个字中的4个字节循环左移1个字节。即将输入字[b0, b1, b2, b3]变换成[b1,b2,b3,b0]。
                b.字节代换：对字循环的结果使用S盒进行字节代换。
                c.轮常量异或：将前两步的结果同轮常量Rcon[j]进行异或，其中j表示轮数。
         * */
        if (i % 4 != 0) {
            for (int j = 0; j < 4; j++) {
                W[i][j] = W[i - 4][j] ^ W[i - 1][j];
            }
        } else {
            uint8_t TW[4];
            for (int j = 0; j < 4; j++) {
                TW[j] = W[i - 1][j];
            }
            left_move(TW, 1);
            sbox_replace_arr(TW);
            int round_count = i / 4;
            for (int j = 0; j < 4; j++) {
                TW[j] = TW[j] ^ rcon[round_count - 1][j];
                W[i][j] = W[i - 4][j] ^ TW[j];
            }
        }
    }

/*    for (int i = 0; i < 44; i++) {
        for (int j = 0; j < 4; ++j) {
            printf("%02X ", W[i][j]);
        }
        puts("");
    }*/
}

// 1. 字节替换操作
void sbox_replace_matrix(uint8_t state_matrix[4][4]) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            uint8_t element = state_matrix[i][j];
            //字节的高4位作为行值，低4位作为列值
            uint8_t row = (element & 0xF0) >> 4;
            uint8_t column = element & 0x0F;
            //取出S盒对应的元素
            uint8_t new_element = sbox[row][column];
            //替换原矩阵中的元素
            state_matrix[i][j] = new_element;
        }
    }
}

// 逆 字节替换操作
void invsbox_replace_matrix(uint8_t state_matrix[4][4]) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            uint8_t element = state_matrix[i][j];
            //字节的高4位作为行值，低4位作为列值
            uint8_t row = (element & 0xF0) >> 4;
            uint8_t column = element & 0x0F;
            //取出S盒对应的元素
            uint8_t new_element = inv_sbox[row][column];
            //替换原矩阵中的元素
            state_matrix[i][j] = new_element;
        }
    }
}

// 2. 行移位操作
void shift_row(uint8_t state_matrix[4][4]) {
    for (int i = 0; i < 4; i++) {
        left_move(state_matrix[i], i);
    }
}

// 逆 行移位操作
void inv_shift_row(uint8_t state_matrix[4][4]) {
    for (int i = 0; i < 4; i++) {
        right_move(state_matrix[i], i);
    }
}

// 3 列混合
void mix_column(uint8_t state_matrix[4][4]) {
    uint8_t tmp[4][4];
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            tmp[i][j] = state_matrix[i][j];
        }
    }
    //列混合变换对 state_matrix 中的每列进行独立操作
    //它把每一个列都看成GF(2^8)中的一个多项式s(x)，在于GF(2^8)上的固定多项式a(x)={03}x^3+{01}x^2+{01}x+{02}进行x^4+1的乘法运算
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            state_matrix[i][j] = GMul(M[i][0], tmp[0][j]) ^
                                 GMul(M[i][1], tmp[1][j]) ^
                                 GMul(M[i][2], tmp[2][j]) ^
                                 GMul(M[i][3], tmp[3][j]);
        }
    }
}

// 逆 列混合
void inv_mix_column(uint8_t state_matrix[4][4]) {
    uint8_t tmp[4][4];
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            tmp[i][j] = state_matrix[i][j];
        }
    }
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            state_matrix[i][j] = GMul(inv_M[i][0], tmp[0][j]) ^
                                 GMul(inv_M[i][1], tmp[1][j]) ^
                                 GMul(inv_M[i][2], tmp[2][j]) ^
                                 GMul(inv_M[i][3], tmp[3][j]);
        }
    }
}

// 4. 加密过程
// 4.1 轮秘钥加
void round_key_add(uint8_t state_matrix[4][4], int round_count) {
    // round_count 第几轮
    // 轮密钥加是将128位轮密钥 W[4 * round_count + i][j] 同状态矩阵中的数据进行逐位异或操作
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            state_matrix[i][j] = state_matrix[i][j] ^ W[4 * round_count + i][j];
//            printf("%02X ", W[4 * round_count + i][j]);
        }
//        puts("");
    }
}

// 4.2 对块进行加密
void block_encrypt(uint8_t message_text[16], uint8_t state_matrix[4][4]) {
    // 秘密扩展
//    key_expansion(key_text);
    //明文转化成矩阵
    text_to_matrix(message_text, state_matrix);

    //开始加密
    //在第一轮迭代之前，先将明文和原始密钥进行一次异或加密操作。
    round_key_add(state_matrix, 0);

    //加密的第1轮到第9轮的轮函数一样，包括4个操作：字节代换、行位移、列混合和轮密钥加。
    for (int i = 1; i <= 9; i++) {
        // 字节代换
        sbox_replace_matrix(state_matrix);
        // 行位移
        shift_row(state_matrix);
        // 列混合
        mix_column(state_matrix);
        // 轮密钥加
        round_key_add(state_matrix, i);
    }

    //最后一轮迭代不执行列混合
    // 字节代换
    sbox_replace_matrix(state_matrix);
    // 行位移
    shift_row(state_matrix);
    // 轮密钥加
    round_key_add(state_matrix, 10);

//    print_matrix(state_matrix);
//    uint8_t text[16];
//    matrix_to_text(ret, state_matrix);

//    return text;
//    printf("text:\n%s\n", text);
//    for (int i = 0; i < 16; ++i) {
//        printf("%02X", text[i]);
//    }
}

// 对块进行解密
void block_decrypt(uint8_t message_text[16], uint8_t state_matrix[4][4]) {
/*    // 秘钥扩展
    key_expansion(key_text);*/
    // 秘文矩阵
//    uint8_t state_matrix[4][4];
    text_to_matrix(message_text, state_matrix);

    // 轮密钥加
    round_key_add(state_matrix, 10);
    // 逆 行位移
    inv_shift_row(state_matrix);
    // 逆 字节代换
    invsbox_replace_matrix(state_matrix);

    for (int i = 9; i >= 1; i--) {
        // 轮密钥加
        round_key_add(state_matrix, i);
        // 逆 列混合
        inv_mix_column(state_matrix);
        // 逆 行位移
        inv_shift_row(state_matrix);
        // 逆 字节代换
        invsbox_replace_matrix(state_matrix);
    }

    round_key_add(state_matrix, 0);
//    puts("decrypt");
//    print_matrix(state_matrix);
//    matrix_to_text(ret, state_matrix);
}
