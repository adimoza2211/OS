#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>

// ELF Header structure
typedef struct {
    uint8_t  e_ident[16]; // ELF identification
    uint16_t e_type;      // Object file type
    uint16_t e_machine;   // Machine type
    uint32_t e_version;   // Object file version
    uint32_t e_entry;     // Entry point address
    // ... Other fields not needed for loading
} Elf32_Ehdr;

// ELF Program Header structure
typedef struct {
    uint32_t p_type;   // Segment type
    uint32_t p_offset; // Segment file offset
    uint32_t p_vaddr;  // Segment virtual address
    uint32_t p_paddr;  // Segment physical address
    uint32_t p_filesz; // Segment size in file
    uint32_t p_memsz;  // Segment size in memory
    uint32_t p_flags;  // Segment flags
    uint32_t p_align;  // Segment alignment
} Elf32_Phdr;

void load_elf(const char* filename) {
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    // Read the ELF header
    Elf32_Ehdr ehdr;
    if (read(fd, &ehdr, sizeof(Elf32_Ehdr)) != sizeof(Elf32_Ehdr)) {
        perror("read");
        close(fd);
        exit(EXIT_FAILURE);
    }

    // Check if it's a valid ELF file
    if (memcmp(ehdr.e_ident, "\x7F""ELF", 4) != 0) {
        fprintf(stderr, "Not an ELF file\n");
        close(fd);
        exit(EXIT_FAILURE);
    }

    // Iterate through program headers and load each segment
    for (int i = 0; i < ehdr.e_phnum; i++) {
        Elf32_Phdr phdr;
        if (lseek(fd, ehdr.e_phoff + i * ehdr.e_phentsize, SEEK_SET) == -1) {
            perror("lseek");
            close(fd);
            exit(EXIT_FAILURE);
        }
        if (read(fd, &phdr, sizeof(Elf32_Phdr)) != sizeof(Elf32_Phdr)) {
            perror("read");
            close(fd);
            exit(EXIT_FAILURE);
        }

        // Only load loadable segments
        if (phdr.p_type == 1) {
            // Allocate memory for the segment
            void* segment = mmap(
                (void*)phdr.p_vaddr,
                phdr.p_memsz,
                PROT_READ | PROT_WRITE | PROT_EXEC,
                MAP_PRIVATE | MAP_FIXED | MAP_ANONYMOUS,
                -1, 0
            );
            if (segment == MAP_FAILED) {
                perror("mmap");
                close(fd);
                exit(EXIT_FAILURE);
            }

            // Read the segment data into memory
            if (lseek(fd, phdr.p_offset, SEEK_SET) == -1) {
                perror("lseek");
                close(fd);
                exit(EXIT_FAILURE);
            }
            if (read(fd, segment, phdr.p_filesz) != phdr.p_filesz) {
                perror("read");
                close(fd);
                exit(EXIT_FAILURE);
            }
        }
    }

    // Close the file and execute the program
    close(fd);
    ((void (*)(void))(uintptr_t)ehdr.e_entry)();
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <elf_filename>\n", argv[0]);
        return EXIT_FAILURE;
    }

    load_elf(argv[1]);

    return 0; // This should never be reached
}
