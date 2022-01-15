#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#include "./so_base.h"

size_t getLibcbase(int pid, char *so_name, char so_path[]) {
    size_t libcAddr;
    char *buf;
    char *end;
    char mapfile[0x18];
    sprintf(mapfile, "/proc/%d/maps", pid);
    FILE *fd = fopen(mapfile, "r");
    if (!fd) {
        printf("open maps error!\n");
        exit(1);
    }
    // search the lib.....
    buf = (char *) malloc(0x100);
    do {
        fgets(buf, 0x100, fd);
    } while (!strstr(buf, so_name));
    // get lib address of start
    end = strchr(buf, '-');
    libcAddr = strtol(buf, &end, 16);
    printf("The process %d's libcbase is: 0x%lx\n", pid, libcAddr);

    // get lib path
    char *start_path = strchr(buf, '/');
    char *end_path = strstr(buf, ".so");
    unsigned long len = end_path - start_path + 3;
    strncpy(so_path, start_path, len);
    fclose(fd);

    return libcAddr;
}

