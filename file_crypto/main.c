#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <dirent.h>
#include <stdint.h>
#include <stdlib.h>
#include "aes128_crypt.h"

enum CryptoType {
    encrypt, decrypt
};
enum CryptoType ctype;

// 根据路径获取目录里的内容
void get_message(char *path, Message **msg) {
    struct stat buf;
    stat(path, &buf);

    *msg = (Message *) calloc(1, sizeof(Message));
    (*msg)->len = buf.st_size;
    (*msg)->message = (uint8_t *) calloc((*msg)->len, sizeof(uint8_t));

    int fd = open(path, O_RDONLY, S_IRWXU | S_IRWXG | S_IRWXO);
    size_t size = read(fd, (*msg)->message, (*msg)->len);
    if (size < 0) {
        perror("open file fail!");
        exit(1);
    }
    close(fd);
}

// 把数据写到指定路径的目录里,如果目录不存在则先创建目录再写入内容
void write_message(char *path, Message *msg) {
    int fd = open(path, O_WRONLY | O_CREAT, S_IRWXU | S_IRWXO);
    if (fd < 0) {
        perror("create directory fail!");
        exit(-1);
    }
    size_t size = write(fd, msg->message, msg->len);
    if (size < 0) {
        perror("open file fail!");
        exit(1);
    }
    close(fd);
}

//uint8_t key[16] = {0x3C, 0xA1, 0x0B, 0x21, 0x57, 0xF0, 0x19, 0x16, 0x90, 0x2E, 0x13, 0x80, 0xAC, 0xC1, 0x07,
//                   0xBA};
uint8_t key[16] = "aaaabbbbccccdddd";

// 生成加密目录
void encrypt_directory(char *path) {
    Message *msg = NULL;
    get_message(path, &msg);
    if (msg->len < 1) {
        printf("file content is null");
        return;
    }
    printf("origin text len = 0x%lX\n",msg->len);
    for (int i = 0; i < msg->len; ++i) {
        printf("0x%02X ", *(msg->message + i));
    }
    puts("");
    AES_CBC_encrypt(key, msg);
    printf("encrypt_directory len = 0x%lX ciphertext:\n", msg->len);
/*    for (int i = 0; i < msg->len; ++i) {
        printf("0x%02X ", *(msg->message + i));
    }
    puts("");*/
    char *new_path = (char *) calloc(256, 1);
    strcat(new_path, path);
    strcat(new_path, ".temp");
    write_message(new_path, msg);
    // 判断文件是否存在
    if (access(path, F_OK) == 0) {
        int ret = remove(path);//删除文件
        if (ret < 0) {
            perror("delete directory fail!");
            exit(1);
        }
    }
    free(new_path);
}

// 生成解密目录
void decrypt_directory(char *path) {
    Message *msg = NULL;
    get_message(path, &msg);
    if (msg->len < 1) {
        printf("file content is null");
        return;
    }
//    printf("decrypt_directory origin text = %s\n", msg->message);
    printf("decrypt_directory origin text len = 0x%lX\n", msg->len);
    AES_CBC_decrypt(key, msg);
    printf("decrypt_directory len = 0x%lX plaintext:\n", msg->len);
    for (int i = 0; i < msg->len; ++i) {
        printf("0x%02X ", *(msg->message + i));
    }
    puts("");

    char *new_path = (char *) calloc(512, 1);
    strncpy(new_path, path, strlen(path) - 5);
//    printf("new_path : %s\n", new_path);
    write_message(new_path, msg);
    if (access(path, F_OK) == 0) {
        int ret = remove(path);//删除文件
        if (ret < 0) {
            perror("delete directory fail!");
            exit(1);
        }
    }
    free(new_path);
}

void readFileList(char *basePath) {
    DIR *dir;
    struct dirent *ptr;

    if ((dir = opendir(basePath)) == NULL) {
        printf("basePath:%s\n", basePath);
        perror("Open dir error...");
        exit(1);
    }

    while ((ptr = readdir(dir)) != NULL) {
        if (strcmp(ptr->d_name, ".") == 0 || strcmp(ptr->d_name, "..") == 0) {    ///current dir OR parrent dir
            continue;
        } else if (ptr->d_type == 8 || ptr->d_type == 10) {    ///file or link file
            //      printf("d_name:%s/%s\n",basePath,ptr->d_name);
            char *current_path = (char *) calloc(512, 1);
            strcat(current_path, basePath);
            strcat(current_path, "/");
            strcat(current_path, ptr->d_name);
            printf("readFileList current_path:\n %s\n", current_path);
            char *suffix = strrchr(ptr->d_name, '.');
            if (ctype == encrypt) {
                if (!(suffix != NULL && (strcmp(suffix, ".temp") == 0))) {
                    encrypt_directory(current_path);
                }
            } else if (ctype == decrypt) {
                if (suffix != NULL && (strcmp(suffix, ".temp") == 0)) {
                    decrypt_directory(current_path);
                }
            }
/*            char *suffix = strrchr(ptr->d_name, '.');
            if (ctype == encrypt && suffix != NULL && (strcmp(suffix, ".temp") != 0)) {
                encrypt_directory(current_path);
            } else if (ctype == decrypt && (strcmp(suffix, ".temp") == 0)) {
                decrypt_directory(current_path);
            }
            free(current_path);*/
        } else if (ptr->d_type == 4) {    ///dir
            char *base = (char *) calloc(512, 1);
            strcpy(base, basePath);
            strcat(base, "/");
            strcat(base, ptr->d_name);
            readFileList(base);
            free(base);
        }
    }
    closedir(dir);
}

char path_tmp[] = "/home/dio/Repositories/test";

// 测试
void encrypt_func() {
    char *path = (char *) malloc(256);
    strncpy(path, path_tmp, sizeof(path_tmp));
    ctype = encrypt;
    readFileList(path);
    free(path);
}

// 测试
void decrypt_func() {
    char *path = (char *) malloc(256);
    strncpy(path, path_tmp, sizeof(path_tmp));
    ctype = decrypt;
    readFileList(path);
    free(path);
}

int main(int argc, char *argv[]) {
//    encrypt_func();
    decrypt_func();

/*    char *path = (char *) calloc(256, 1);
    int c = 0;
    while ((c = getopt(argc, argv, "edp:k:")) != -1) {
        switch (c) {
            case 'e': {
                ctype = encrypt;
            }
                break;
            case 'd': {
                ctype = decrypt;
            }
                break;
            case 'p': {
                strncpy(path, optarg, strlen(optarg));
            }
                break;
            case 'k': {
                if (strlen(optarg) < 16) {
                    puts("crypto key is too short!");
                    return -1;
                }
                strncpy(key, optarg, 16);
            }
                break;
            default: {
                printf("not find in optstring, optarg = %s\n", optarg);
                return -1;
            }
        }
    }
    readFileList(path);*/

    return 0;
}