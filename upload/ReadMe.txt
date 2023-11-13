Functionalities:

For vector sum:
1. The function divides the range [low, high] into chunks based on the number of threads (numThreads).
2. For each chunk a thread is created. In pthread_create , another function called chunk_adder is called which executes the lambda function on the specified range.
3. The main thread waits for the completion of all the threads using pthread_join.
4. The function measures the execution time in microseconds using the difftime function.
5. The execution time is printed at the end.


Simlar implementation for matrix multiplication parellel_for , here chunk size is calculated on start and end row index.
Each thread iterates through specified row and coloumn range and calls the lambda function on each chunk.


Private repo link:https://github.com/adimoza2211/OS.git

Contributions:

Aditya Moza: Parellel_for function for matrix multiplication.
Aarya Khandelwal: Parellel_for function for vector sum.

