#include <stdint.h>

#ifndef AES128_AES_STRUCT_H
#define AES128_AES_STRUCT_H

typedef struct {
    uint8_t *message;
    unsigned long len;
} Message;

#endif //AES128_AES_STRUCT_H
