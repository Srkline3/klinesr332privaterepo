/* Copyright 2016 Rose-Hulman Institute of Technology

Here is some code that factors in a super dumb way.  We won't be
attempting to improve the algorithm in this case (though that would be
the correct thing to do).

Modify the code so that it starts the specified number of threads and
splits the computation among them.  You can be sure the max allowed 
number of threads is 50.  Be sure your threads actually run in parallel.

Your threads should each just print the factors they find, they don't
need to communicate the factors to the original thread.

ALSO - be sure to compile this code with -pthread or just used the 
Makefile provided.

 */
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>

int numThreads;
unsigned long long int target;

void *threadFactor(void *arg){
    
    int start = *(int*) arg;

    // printf("Target: %llu, NumThreads: %d, Start: %d\n", target, numThreads, start);
    long long unsigned int i;
    for(i = start; i <= target/2; i = i + numThreads) {
      printf("testing %llu\n", i);
      if (target % i == 0) {
        printf("%llu is a factor\n", i);
      }
  }
  return NULL;
}

int main(void) {
  /* you can ignore the linter warning about this */
  // unsigned long long int i, start = 0;
  
  printf("Give a number to factor.\n");
  scanf("%llu", &target);

  printf("How man threads should I create?\n");
  scanf("%d", &numThreads);
  if (numThreads > 50 || numThreads < 1) {
    printf("Bad number of threads!\n");
    return 0;
  }

  pthread_t myThreads[numThreads];
  int start[numThreads];

  for(int j = 0; j<numThreads; j++){
    start[j] = j+2;
    pthread_create(&myThreads[j], NULL, threadFactor, &start[j]);
  }

  for(int k = 0; k < numThreads; k++){
    pthread_join(myThreads[k], NULL);
  }

  printf("Main: Done factoring\n");
  return 0;
}

