#include "SimpleSmartLoader.h"
#define MAP_ANONYMOUS	0x20

off_t fileSize;
int fd;
Elf32_Ehdr *ehdr;
int globalNPAGEcount=0;
int numSegfaults = -1;
size_t fragmentation = 0;
void* lastPageFaultedSegmentvaddr = NULL;

struct cleanup{
    void* address;
    int numpages;
};
struct cleanup arr[10];

void loader_cleanup()
{
    //iterates through the array and deallocates memory for each segment.
    for(int i = 0; i <= numSegfaults; i++){
        munmap(arr[i].address,arr[i].numpages*PAGESZ);
    }
    free(ehdr);
}

Elf32_Phdr returnPtype(void* fault_address)
{
    Elf32_Phdr temp;
    off_t programheadertable = ehdr->e_phoff;//stores offset of the elf header table.

    // finds the program header address in which the page fault occured.
    for(int i = 0; i < ehdr->e_phnum; i++)
    {
        if(lseek(fd,programheadertable + i*ehdr->e_phentsize,SEEK_SET)== -1){
            perror("lseek fail");
            exit(EXIT_FAILURE);
        }
        if(read(fd,&temp,sizeof(Elf32_Phdr)) != sizeof(Elf32_Phdr)){ //checks for size of segement read.
            perror("read fail");
            exit(EXIT_FAILURE);
        }
        if(fault_address >= (void*)temp.p_vaddr && fault_address < (void*)(temp.p_vaddr + temp.p_memsz)){  //checks the range for the fault address.
            return temp;
        }
    }
}





void sigsegv_handler(int signum, siginfo_t* info, void* context){
    
    numSegfaults++;
    void* fault_address = info->si_addr;// the address of the line where seg fault occured is saved in fault address.

    Elf32_Phdr temp= returnPtype(fault_address);// returns the program header address where memory has to be allocated.
    int npages;
    off_t remain = temp.p_memsz%PAGESZ;

    // calculates the no of pages to be assigned for the program header. 
    if(remain== 0){
        npages= temp.p_memsz/PAGESZ;
    }
    else {
        npages= (temp.p_memsz/PAGESZ) + 1;
        // fragmentation += PAGESZ-remain;
    }


    globalNPAGEcount++; // increments the pagecounts by only 1, as at a time only one page is allocated for any phdr.
    void* offset = (void*)((uintptr_t)(fault_address - temp.p_vaddr) / PAGESZ);
    void* allocated_memory = mmap((void*)(temp.p_vaddr + (uintptr_t)offset*PAGESZ) , //mmap 1 page memory for the segment.
        PAGESZ,
        PROT_READ | PROT_WRITE | PROT_EXEC,
        MAP_ANONYMOUS |MAP_PRIVATE,
        -1, 0);

    //appending the fault addresses and npages in an array for cleanup later.
    arr[numSegfaults].address = (void*)fault_address;
    arr[numSegfaults].numpages = npages;

    //lseek the offset and allocate memory finally.
    if(lseek(fd,temp.p_offset + (uintptr_t)offset * PAGESZ,SEEK_SET) == -1){//exiting if lseek failed
      close(fd);
      exit(EXIT_FAILURE);
    }
    if (read(fd,allocated_memory,PAGESZ)==-1){ //exiting if read failure.
        close(fd);
        exit(EXIT_FAILURE);
    }
        
   
    printf("fault addr is %p\n", fault_address);
   
   //calculates the fragmentation for each allocation. 
    if(lastPageFaultedSegmentvaddr == NULL){
        fragmentation += npages*PAGESZ - temp.p_memsz;// increments the fragementation for the segment allocated wrt npages.
        lastPageFaultedSegmentvaddr = (void*)temp.p_vaddr;//sets last page fault address as the current phdr address.
    }
    else if(lastPageFaultedSegmentvaddr != (void*)temp.p_vaddr){ //checks if the last page fault address is not similar to current phdr address. 
        fragmentation += npages*PAGESZ - temp.p_memsz;           // as fragemtation for a segement will only increment once.
        lastPageFaultedSegmentvaddr = (void*)temp.p_vaddr;
    }
    fprintf(stderr, "Segmentation Fault (SIGSEGV) received. Exiting.\n");
    return;
}

void load_and_run_elf(char** exe)
{
    struct sigaction sa;
    sa.sa_sigaction = sigsegv_handler;//handler for segmentation fault is set as sigsev_handler.
    sa.sa_flags = SA_SIGINFO;//flags  stores the info related to the seg fault( like the address at which fault occured).
    if(sigaction(SIGSEGV,&sa,NULL) == -1){
        perror("sigaction");
        return ;
    }

    fd = open(exe[1], O_RDONLY);   
    struct stat fileStatus;
    if(fstat(fd, &fileStatus) == -1){//checks status of file
        close(fd);
        exit(EXIT_FAILURE);
    }
    fileSize = fileStatus.st_size;
    ehdr=(Elf32_Ehdr*)malloc(sizeof(Elf32_Ehdr));
    read(fd,ehdr,sizeof(Elf32_Ehdr));//reads a segment of the elf file -> leads to page fault

    // for(int i = 0; i < ehdr->e_phnum; i++){
    //     if(lseek(fd,(ehdr->e_phoff) + (i*ehdr->e_phentsize), SEEK_SET) == -1){//exiting program if lseek fails
    //         close(fd);
    //         exit(EXIT_FAILURE);
    //     }
    //     if(read(fd,&phdr,sizeof(Elf32_Phdr)) != sizeof(Elf32_Phdr)){//exiting program if size mismatch
    //         close(fd);
    //         exit(EXIT_FAILURE);
    //     }
    //     if(phdr.p_type == 1 && ehdr->e_entry >= phdr.p_vaddr && ehdr->e_entry < (phdr.p_vaddr+phdr.p_memsz )){//found entrypoint 
    // printf("found\n");
    //         entry_phdr = &phdr;
    //         break;
    //     }
    // }
    // int* ptr = (int*)0x11111111;
    // *ptr = 5;
    int(*_start)() = (int(*)())(ehdr->e_entry);//typecasting the entry point address to _start
    int result = _start();
    printf("----------------------------------------------------\n");
    printf("User _start return value = %d\n",result);
    printf("Number of segfaults is %i\n", ++numSegfaults);
    printf("No of pages used is %i\n", globalNPAGEcount);
    printf("Fragmenation is %zu bytes \n",fragmentation);
    close(fd);
}


int main(int argc, char** argv) 
{
    if(argc != 2) {
        printf("Usage: %s <ELF Executable> \n",argv[0]);
        exit(1);
    }
    load_and_run_elf(argv);  //runs the executable
    return 0;
}