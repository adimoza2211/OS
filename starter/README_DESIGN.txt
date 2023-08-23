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

loader cleanup unmaps allocated memory at the end.

Contributions:
Aditya Moza: The first three steps for the simple loader including opening the file, iterating through the phdr and allocating p_memsz.

Aarya Khandelwal: The last three steps for the simple loader including loading entry_point segment to the memory , typecasting the pointer to start type and calling _start function . 

The inclusion of the makefiles of loader/launcher was done by both.

Erros, Segmentation Faults were fixed by both 

Link to private github repo:https://github.com/adimoza2211/OS.git

STEPS TO RUN THE PROGRAM(in the with-bonus dir):
make clean
make
make run


STEPS TO RUN THE PROGRAM(in the without-bonus dir):
make clean
make 
make run
