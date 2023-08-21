#include "loader.h"

Elf32_Ehdr *ehdr;
Elf32_Phdr *phdr;
int fd;

/*
 * release memory and other cleanups
 */
void loader_cleanup() {
  
  
}

/*
 * Load and run the ELF executable file
 */
void load_and_run_elf(char** exe) {
  fd = open(argv[1], O_RDONLY);
  // 1. Load entire binary content into the memory from the ELF file.
  // 2. Iterate through the PHDR table and find the section of PT_LOAD 
  //    type that contains the address of the entrypoint method in fib.c
  // 3. Allocate memory of the size "p_memsz" using mmap function 
  //    and then copy the segment content
  // 4. Navigate to the entrypoint address into the segment loaded in the memory in above step
  // 5. Typecast the address to that of function pointer matching "_start" method in fib.c.
  // 6. Call the "_start" method and print the value returned from the "_start"
  int result = _start();
  printf("User _start return value = %d\n",result);
}


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
   print("Second Byte Incorrect\n");
   return 0;
 }
 if(header->e_ident[EI_MAG3] != ELFMAG3) {
   print("Third Byte Incorrect\n");
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
  loader_cleanup();
  return 0;
}
