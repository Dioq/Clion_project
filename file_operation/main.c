#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include "common.h"

// 每块文件大小 100 M
unsigned long block_size = 1024 * 1024 * 100;

// 把数据写到指定路径的目录里,如果目录不存在则先创建目录再写入内容
void write_message(char *path, char *msg, unsigned long len) {
    int fd = open(path, O_WRONLY | O_CREAT, S_IRWXU | S_IRWXO);
    if (fd < 0) {
        perror("create directory fail!");
        exit(-1);
    }
    int size = write(fd, msg, len);
    if (size < 0) {
        perror("open file fail!");
        exit(1);
    }
    close(fd);
}

// 根据路径获取目录里的内容
void get_message(char *path, Message **msg) {
    struct stat buf;
    stat(path, &buf);

    *msg = (Message *) calloc(1, sizeof(Message));
    (*msg)->len = buf.st_size;
    (*msg)->message = (char *) malloc((*msg)->len);

    int fd = open(path, O_RDONLY, S_IRWXU | S_IRWXG | S_IRWXO);
    int size = read(fd, (*msg)->message, (*msg)->len);
    if (size < 0) {
        perror("open file fail!");
        exit(1);
    }
    close(fd);
}

void split_file(char *currentPath, char *fileName) {
    char *filePath = (char *) calloc(512, 1);
    strcpy(filePath, currentPath);
    strcat(filePath, "/");
    strcat(filePath, fileName);

    struct stat buf;
    stat(filePath, &buf);
    // 文件大小
    unsigned long len = buf.st_size;

    // 如果文件小于分块大小 就不用分块了
    if (len <= block_size) {
        return;
    }

    int fd = open(filePath, O_RDONLY, S_IRWXU | S_IRWXG | S_IRWXO);
    char *data = (char *) malloc(len);
    int size = read(fd, data, len);
    if (size < 0) {
        perror("open file fail!");
        exit(-1);
    }

    // 分块后的 命名，由于使用C语言，写起来有些麻烦
    char *new_file_name = (char *) calloc(128, 1);
    strcat(new_file_name, fileName);
    strcat(new_file_name, "^split_");
    // 分块个数
    unsigned long block_num = len / block_size;
//    char *block_num_ptr = (char *)&block_num;
    char block_num_tmp[9];
    sprintf(block_num_tmp, "%08lX", block_num);
    strcat(new_file_name, block_num_tmp);
    // 最后一块大小
    unsigned long last_size = len % block_size;

    for (unsigned long i = 0; i < block_num; ++i) {
        unsigned long size_tmp = block_size;
        if (i == (block_num - 1)) {
            size_tmp = last_size;
        }

        char *name_tmp = (char *) calloc(128, 1);
        strcat(name_tmp, new_file_name);

//        char block_size_tmp[9];
//        sprintf(block_size_tmp, "%08lX", size_tmp);
//        strcat(name_tmp, "_");
//        strcat(name_tmp, block_size_tmp);

        char index_tmp[8];
        sprintf(index_tmp, "%05lX", i);
        strcat(name_tmp, "_");
//        char *index_ptr = (char *)&i;
        strcat(name_tmp, index_tmp);

//        puts(name_tmp);
        char *new_path = (char *) calloc(256, 1);
        strcat(new_path, currentPath);
        strcat(new_path, "/");
        strcat(new_path, name_tmp);
        puts(new_path);

        char *ptr = data + (block_size * i);
        write_message(new_path, ptr, size_tmp);
    }
}

void merge_file(char *currentPath, char *fileName) {
    char *name_tmp = (char *) calloc(128, 1);
    strcpy(name_tmp, fileName);
//    printf("name_tmp : %s\n", name_tmp);
    char *origin_name = strsep(&name_tmp, "^");
//    printf("origin_name : %s\n", origin_name);
//    printf("len = %lu\n", sizeof(name_tmp));
    // 判断是否是自己分割的文件
    if (origin_name == NULL || strlen(name_tmp) != 20) {
        return;
    }
//    printf("split_symbol : %s\n",name_tmp);
    // 进一步判断
    char *buffix = strsep(&name_tmp, "_");
    if (buffix != NULL && strcmp(buffix, "split") != 0) {
        return;
    }
//    printf("last str : %s\n", name_tmp);
    char *block_num_str = strsep(&name_tmp, "_");
//    unsigned long block_num = (unsigned long) *block_num_str;
//    char *block_size_str = strsep(&name_tmp, "_");
    long block_num = strtol(block_num_str, NULL, 8);
//    char *index_str = name_tmp;
//    unsigned long index = (unsigned long) *index_str;
//    long index = strtol(index_str, NULL, 8);
//    printf("block_num_str = %s, index_str = %s\n", block_num_str, index_str);
//    printf("block_num = %lx, index = %lx\n", block_num, index);
    char *target_data = (char *) malloc(block_num * block_size);
//    unsigned long last_block_size = 0;
    for (unsigned long i = 0; i < block_num; ++i) {
        char *every_file_name = (char *) calloc(128, 1);
        strcat(every_file_name, origin_name);
        strcat(every_file_name, "^");
        strcat(every_file_name, "split");
        strcat(every_file_name, "_");
        strcat(every_file_name, block_num_str);
        strcat(every_file_name, "_");
        char index_tmp[8];
        sprintf(index_tmp, "%05lu", i);
        strcat(every_file_name, index_tmp);
//        printf("every file name:\n %s\n", every_file_name);

        // 获取待合并的文件的全路径
        char *every_file_path = (char *) calloc(256, 1);
        strcat(every_file_path, currentPath);
        strcat(every_file_path, "/");
        strcat(every_file_path, every_file_name);
        printf("every file path:\n %s\n", every_file_path);
        struct stat buf;
        stat(every_file_path, &buf);
        char *every_block = (char *) malloc(buf.st_size);


/*        if (i == (block_num - 1)) {
            struct stat buf;
            stat(every_file_path, &buf);
            last_block_size = buf.st_size;
        }*/

    }
}

void readFileList(char *basePath) {
    DIR *dir;
    struct dirent *ptr;

    if ((dir = opendir(basePath)) == NULL) {
        perror("Open dir error...");
        exit(1);
    }

    while ((ptr = readdir(dir)) != NULL) {
        if (strcmp(ptr->d_name, ".") == 0 || strcmp(ptr->d_name, "..") == 0)    ///current dir OR parrent dir
            continue;
        else if (ptr->d_type == 8) { ///file
//            printf("d_name:%s/%s\n", basePath, ptr->d_name);
//            split_file(basePath, ptr->d_name);
            merge_file(basePath, ptr->d_name);
        } else if (ptr->d_type == 10) {  ///link file
            printf("d_name:%s/%s\n", basePath, ptr->d_name);
        } else if (ptr->d_type == 4)    ///dir
        {
//            char base[256];
//            memset(base, '\0', sizeof(base));
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

int main() {
    readFileList("/home/dio/Desktop/test");
    return 0;
}
