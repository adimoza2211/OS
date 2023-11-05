Functionalities of the SimpleSmartLoader

1. Executable is run and a handler for segmentation/page fault is set.

2. Set the file descriptor and typecasted the entry point adrress to _start.

3. As no allocation for segments has been done page fault occurs and controls shifts to seg fault handler.

4. The address which caused the seg fault is found and the corressponding program header address is stored.

5. No of pages to be allocated for the segment is calculated using PAGE_SIZE.

6. Find the offset and alloate memory for segment using mmap.

7. Increment the global page count and append the phdr in an array for future cleanup.

8. lseek and read the allocated memory.

9. (FOR BONUS PART ONLY) As only one page is allocated at a time the fragmentation is calculated using a  lastPageFaultedSegmentvaddr which stores the last phdr address where page fault occured.
   The fragmentation is incremented only once for a segment (when the first page fault occurs). 
   Thus if the lastPageFaultedSegmentvaddr is similar to the current phdr then  fragmentation is not calculated.

10. The program finally prints the no of segfaults, pages allocated and total fragmentation bytes.



Github repo link:https://github.com/adimoza2211/OS.git


Contributions:


