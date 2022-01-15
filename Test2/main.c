#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char **argv) {
    if (argc < 3) {
        fprintf(stderr, "Usage:\n\t%s pid\n", argv[0]);
        exit(1);
    }

    pid_t target_pid = atoi(argv[1]);
    unsigned long addr = atol(argv[2]);
    printf("addr : %lx\n", addr);

    attach_process(target_pid);

    unsigned long len = 32;
    uint8_t *dst = (uint8_t *) calloc(len,1);
    getdata(target_pid, addr, dst, len);

    printf("dst:%p\nvalue:%s\n", (void *) addr, dst);

    continue_process(target_pid);
    //      int i = 0;
    //      while(1) {
    //              i++;
    //              long value = ptrace(PTRACE_PEEKDATA, target_pid, addr, NULL);
    //              printf("dst:%p, value = %ld\n", (void *) addr, value);
    //              sleep(3);
    //              if(i == 10) {
    //                      break;
    //              }
    //      }

    end_tracke_process(target_pid);

    return 0;
}
