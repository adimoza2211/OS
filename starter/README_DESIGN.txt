SimpleLoader Implementation Details:
The code in loader.c is commented step by step detailing what has been done.

We open the file and use struct stat from sys/stat.h to check for errors and size

Use mmap to load the entire content into memory and assign a typecasted pointer to it 

Use an elfchecker() function to check if the file is in proper ELF format 

iterate through the PHDR table and find the section of PT_LOAD using pointer arithmetic since the lseek function runs from the start to i and jumps
a length of program header entry size each iteration

we check for size mismatch and lseek error in each iteration

allocate memory for segment checking for mmap errors and size mismatch on each step

navigate to the entry point field of ehdr

typecast and call _start()

loader cleanup unmaps allocated memory at the end



Link to private github repo:





