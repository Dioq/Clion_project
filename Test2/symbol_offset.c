#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <elf.h>

#include <sys/user.h>
#include <sys/stat.h>
#include <sys/mman.h>

typedef struct handle {
    Elf64_Ehdr *ehdr;
    Elf64_Phdr *phdr;
    Elf64_Shdr *shdr;
    uint8_t *mem;
    char *symname;    // target function name
    Elf64_Addr symaddr; // target function address
    struct user_regs_struct pt_reg;
    char *path; // elf file path
} handle_t;

/**
 * 根据符号名查找符号地址
 */
void lookup_symbol(handle_t *h) {
    unsigned long i, j, NumOfSym;
    char *strtab;
    Elf64_Sym *symtab;

    for (i = 0; i < h->ehdr->e_shnum; i++) {
        /* 找 .symtab 节，该节保存了符号信息，每一个符号项为Elf64_Sym */
        if (h->shdr[i].sh_type == SHT_SYMTAB) {
            /* 类型为SHT_SYMTAB的节，其sh_link为字符串表所在节表中的下标 */
            /* 因此h->shdr[i].sh_link为字符串表下标 */
            strtab = (char *) (h->mem + h->shdr[h->shdr[i].sh_link].sh_offset);
            /* 获取符号表的首地址 */
            symtab = (Elf64_Sym *) &h->mem[h->shdr[i].sh_offset];

            NumOfSym = h->shdr[i].sh_size / sizeof(Elf64_Sym);
            for (j = 0; j < NumOfSym; j++) {
//                char *name = (char *) &strtab[symtab->st_name];
//                printf("\nsymbol name : %s\n", name);
                /* st_name为符号名在字符串表中的下标 */
                if (!strncmp(&strtab[symtab->st_name], h->symname, strlen(h->symname))) {
                    /* st_value 为符号的地址 */
                    h->symaddr = symtab->st_value;
                    break;
                }
                symtab++;
            }
        }
    }
}

handle_t h;

void handle_elf() {
    struct stat st;
    /* 打开指定文件 */
    int fd;
    if ((fd = open(h.path, O_RDONLY)) < 0) {
        perror("open");
        exit(-1);
    }

    /* 获取指定文件的属性 */
    if (fstat(fd, &st) < 0) {
        perror("fstat");
        exit(-1);
    }

    /* 将目标文件映射到本进程的虚拟内存中 */
    h.mem = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (h.mem == MAP_FAILED) {
        perror("mmap");
        exit(-1);
    }

    h.ehdr = (Elf64_Ehdr *) h.mem;
    h.phdr = (Elf64_Phdr *) (h.mem + h.ehdr->e_phoff);
    h.shdr = (Elf64_Shdr *) (h.mem + h.ehdr->e_shoff);
}

size_t symbol_address(char *so_path, char *symbol_name) {
    h.path = so_path;
    h.symname = symbol_name;
    handle_elf();
    lookup_symbol(&h);
    return h.symaddr;
}