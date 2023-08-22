#include "loader.h"

Elf32_Ehdr *ehdr;
Elf32_Phdr *phdr;
int fd;
int elfChecker(Elf32_Ehdr* header);

/*
 * release memory and other cleanups
 */
void loader_cleanup(void* entry_segment, void* elfimage, off_t filesize, uint32_t pfilesize) {
  //deallocating entry segment
  munmap(entry_segment,pfilesize);
  munmap(elfimage,filesize);
}

/*
 * Load and run the ELF executable file
 */
void load_and_run_elf(char* exe) {
  fd = open(exe, O_RDONLY);
  if(fd == -1){
    perror("error opening\n");
    exit(EXIT_FAILURE);
  }
  //gnu c manual instructs to use off_t for file size in bytes so that's what I used
  struct stat fileStatus;
  if(fstat(fd, &fileStatus) == -1){
    close(fd);
    exit(EXIT_FAILURE);
  }
  off_t fileSize = fileStatus.st_size;

  // 1. Load entire binary content into the memory from the ELF file.
  void* elfStorage = mmap(NULL,fileSize,PROT_READ,MAP_PRIVATE,fd,0);
  if(elfStorage == MAP_FAILED){
    close(fd);
    exit(EXIT_FAILURE);
  } 
  ehdr =  (Elf32_Ehdr*)elfStorage;
  elfChecker(ehdr);//checking for a valid elf file

  // 2. Iterate through the PHDR table and find the section of PT_LOAD 
  // type that contains the address of the entrypoint method in fib.c
  Elf32_Phdr entry_phdr;
  for(int i = 0; i < ehdr->e_phnum; i++){
    Elf32_Phdr phdr;
    if(lseek(fd,(ehdr->e_phoff) + (i*ehdr->e_phentsize), SEEK_SET) == -1){
      close(fd);
      exit(EXIT_FAILURE);
    }
    if(read(fd,&phdr,sizeof(Elf32_Phdr)) != sizeof(Elf32_Phdr)){
      close(fd);
      exit(EXIT_FAILURE);
    }
    if(phdr.p_type == 1 && ehdr->e_entry >= phdr.p_vaddr && ehdr->e_entry < (phdr.p_vaddr+phdr.p_memsz )){
      entry_phdr = phdr;
      break;
    }
  }  
  
  // 3. Allocate memory of the size "p_memsz" using mmap function 
  //    and then copy the segment content
  void* entry_segment = mmap(
        (void*)entry_phdr.p_vaddr,
        entry_phdr.p_memsz,
        PROT_READ | PROT_WRITE | PROT_EXEC,
        MAP_PRIVATE | MAP_FIXED | MAP_ANONYMOUS,
        -1, 0);
    if (entry_segment == MAP_FAILED) {
        perror("mmap");
        close(fd);
        exit(EXIT_FAILURE);
    }
    //reading segment data into memory
    if(lseek(fd,entry_phdr.p_offset,SEEK_SET) == -1){
      close(fd);
      exit(EXIT_FAILURE);
    }
    if(read(fd,entry_segment,entry_phdr.p_filesz) != entry_phdr.p_filesz){
      close(fd);
      exit(EXIT_FAILURE);
    }
  // 4. Navigate to the entrypoint address into the segment loaded in the memory in above step
    int(*_start)() = (int(*)())ehdr->e_entry;

  // 5. Typecast the address to that of function pointer matching "_start" method in fib.c.
  int result = _start();
  // 6. Call the "_start" method and print the value returned from the "_start"
  printf("User _start return value = %d\n",result);
  loader_cleanup(entry_segment,elfStorage,fileSize,entry_phdr.p_memsz);
}

//checks if the elf file is in correct format
int elfChecker(Elf32_Ehdr* header){
 if(header == NULL) {
   printf("NULL pointer to elf header\n");
   return 0;
 }
 if(header->e_ident[EI_MAG0] != ELFMAG0){//checks if the value of the first byte of the ELF identification array(e_ident) is equal to 0x7f
   printf("First Byte Incorrect\n");
   return 0;
 }
 if(header->e_ident[EI_MAG1] != ELFMAG1){//second byte
   printf("Second Byte Incorrect\n");
   return 0;
 }
 if(header->e_ident[EI_MAG2] != ELFMAG2){//second byte
   printf("Second Byte Incorrect\n");
   return 0;
 }
 if(header->e_ident[EI_MAG3] != ELFMAG3) {
   printf("Third Byte Incorrect\n");
   return 0;
 }
 return 1;
}

int main(int argc, char** argv) 
{
  if(argc != 2) {
    printf("Usage: %s <ELF Executable> \n",argv[0]);
    exit(1);
  }
  // 1. carry out necessary checks on the input ELF file
  // 2. passing it to the loader for carrying out the loading/execution
  load_and_run_elf(argv[1]);
  // 3. invoke the cleanup routine inside the loader  
  return 0;
}