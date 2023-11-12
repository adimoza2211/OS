#include <iostream>
#include <list>
#include <functional>
#include <stdlib.h>
#include <cstring>

void* chunk_adder(void* ptr);
void* matrix_multiply_threaded(void* ptr);
int user_main(int argc, char **argv);

/* Demonstration on how to pass lambda as parameter.
 * "&&" means r-value reference. You may read about it online.
 */
void demonstration(std::function<void()> && lambda) {
  lambda();
}
using Myfunctionpointer = void(*)(int);

typedef struct {
  int low; 
  int high;
  std::function<void(int)>lambda;
}vectorSumArgs ;

typedef struct{
  int row_start;
  int row_end;
  int size;
  std::function<void(int, int)>lambda;
}MatrixMulArg;

void parallel_for(int low, int high,std::function<void(int)>&&lambda, int numThreads)
{
  pthread_t tid[numThreads];
  vectorSumArgs argsArr[numThreads];
  int chunk = 1 + ((high-1)/numThreads);
  for(int i = 0; i < numThreads; i++){
    int jlow = i*chunk; int jhigh = (i+1)*chunk;
    if(jhigh > high){
      jhigh = high;
    }
    argsArr[i].low = jlow;
    argsArr[i].high = jhigh;
    argsArr[i].lambda = lambda;
    pthread_create(&tid[i], NULL,chunk_adder,static_cast<void*>(&argsArr[i]));
  }
  for(int i = 0; i < numThreads; i++){
    pthread_join(tid[i],nullptr);
  }
} 
void* chunk_adder(void* ptr)
{
  vectorSumArgs* args = static_cast<vectorSumArgs*>(ptr);
  for(int i = args->low; i < args->high ; i++){
    args->lambda(i);
  }
  return NULL;
}

void parallel_for(int low1, int high1, int low2, int high2,std::function<void(int, int)> &&lambda, int numThreads)
{
  pthread_t tid[numThreads];
  MatrixMulArg args[numThreads];
  int chunk = 1 + ((high1-1)/numThreads);
  for(int i = 0; i < numThreads; i++){
    int rowst = i*chunk; int rownd = (i+1)*chunk;
    if(rownd > high1){
      rownd = high1;
    }
    args[i].row_start = rowst; 
    args[i].row_end =rownd; 
    args[i].lambda = lambda;
    args[i].size = high1;
    pthread_create(&tid[i],NULL,matrix_multiply_threaded,static_cast<void*>(&args[i]));
  }
  for(int i = 0; i < numThreads; i++){
    pthread_join(tid[i],nullptr);
  }
}

void* matrix_multiply_threaded(void* ptr) {
  MatrixMulArg* arg = static_cast<MatrixMulArg*>(ptr);
  for(int i = arg->row_start; i < arg->row_end; i++){
    for(int j = 0; j < arg->size; j++){
      arg->lambda(i,j);
    }
  }
  return NULL;
}


int main(int argc, char **argv) {
  /* 
   * Declaration of a sample C++ lambda function
   * that captures variable 'x' by value and 'y'
   * by reference. Global variables are by default
   * captured by reference and are not to be supplied
   * in the capture list. Only local variables must be 
   * explicity captured if they are used inside lambda.
   */
  int x=5,y=1;
  // Declaring a lambda expression that accepts void type parameter
  auto /*name*/ lambda1 = /*capture list*/[/*by value*/ x, /*by reference*/ &y](void) {
    /* Any changes to 'x' will throw compilation error as x is captured by value */
    y = 5;
    std::cout<<"====== Welcome to Assignment-"<<y<<" of the CSE231(A) ======\n";
    /* you can have any number of statements inside this lambda body */
  };
  // Executing the lambda function
  demonstration(lambda1); // the value of x is still 5, but the value of y is now 5

  int rc = user_main(argc, argv);
 
  auto /*name*/ lambda2 = [/*nothing captured*/]() {
    std::cout<<"====== Hope you enjoyed CSE231(A) ======\n";
    /* you can have any number of statements inside this lambda body */
  };
  demonstration(lambda2);
  return rc;
}
#define main user_main



