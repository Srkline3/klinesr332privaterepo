#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include<string.h>

// max number of values for each thread to sort
#define MAX_VALS_PER_THREAD 1000
// max threads allowed
#define MAX_N_SIZE 100


/* other global variable instantiations can go here */
struct myData{
  int start;
  int end;
  int size;
  int sortMethod;
  suseconds_t usecs_passed;
};

/* Uses a brute force method of sorting the input list. */
void BruteForceSort(int inputList[], int inputLength) {
  int i, j, temp;
  for (i = 0; i < inputLength; i++) {
    for (j = i+1; j < inputLength; j++) {
      if (inputList[j] < inputList[i]) {
        temp = inputList[j];
        inputList[j] = inputList[i];
        inputList[i] = temp;
      }
    }
  }
}

/* Uses the bubble sort method of sorting the input list. */
void BubbleSort(int inputList[], int inputLength) {
  char sorted = 0;
  int i, temp;
  while (!sorted) {
    sorted = 1;
    for (i = 1; i < inputLength; i++) {
      if (inputList[i] < inputList[i-1]) {
        sorted = 0;
        temp = inputList[i-1];
        inputList[i-1] = inputList[i];
        inputList[i] = temp;
      }
    }
  }
}

/* Merges two arrays.  Instead of having two arrays as input, it
 * merges positions in the overall array by re-ording data.  This 
 * saves space. */
void Merge(int *array, int left, int mid, int right) {
  int tempArray[MAX_VALS_PER_THREAD];
  int pos = 0, lpos = left, rpos = mid;
  while (lpos <= mid && rpos <= right) {
    if (array[lpos] < array[rpos]) {
      tempArray[pos++] = array[lpos++];
    } else {
      tempArray[pos++] = array[rpos++];
    }
  }
  while (lpos < mid)  tempArray[pos++] = array[lpos++];
  while (rpos <= right)tempArray[pos++] = array[rpos++];
  int iter;
  for (iter = 0; iter < pos; iter++) {
    array[iter+left] = tempArray[iter];
  }
  return;
}

/* Divides an array into halfs to merge together in order. */
void MergeSort(int array[], int inputLength) {
  int mid = inputLength/2;
  if (inputLength > 1) {
    MergeSort(array, mid);
    MergeSort(array + mid, inputLength - mid);
    // merge's last input is an inclusive index
    // printf("calling merge 0->%d, 1->%d\n mid %d\n",array[0], array[1], mid); 
    Merge(array, 0, mid, inputLength - 1);
  }
}

// you might want some globals, put them here
int size;
int* data_array; 
suseconds_t brute_min = 1000000;
suseconds_t brute_max = 0;
suseconds_t bubble_min = 1000000;
suseconds_t bubble_max = 0;
suseconds_t merge_min = 1000000;
suseconds_t merge_max = 0;

// here's a global I used you might find useful
char* descriptions[] = {"brute force","bubble","merge"};

// I wrote a function called thread dispatch which parses the thread
// parameters and calls the correct sorting function
//
// you can do it a different way but I think this is easiest
// void* thread_dispatch(void* data) {

// }

void* thread_brute_force(void* data){
  struct timeval startt, stopt;
  struct myData datum = *(struct myData*) data;
  printf("Sorting indexes %d-%d with brute force\n", datum.start, datum.end);
  int toSort[datum.size];
  memcpy(toSort, &data_array[datum.start], datum.size*sizeof(int));


  gettimeofday(&startt, NULL);
  BruteForceSort(toSort, datum.size);
  gettimeofday(&stopt, NULL);

  datum.usecs_passed = stopt.tv_usec - startt.tv_usec;

  printf("Sorting indexes %d-%d with brute force done in %li usecs\n", datum.start, datum.end, datum.usecs_passed);

    if(datum.usecs_passed > brute_max){
    brute_max = datum.usecs_passed;
  }

   if(datum.usecs_passed < brute_min){
    brute_min = datum.usecs_passed;
  }

  return NULL;
}

void* thread_bubble(void* data){
  struct timeval startt, stopt;
  struct myData datum = *(struct myData*) data;
  printf("Sorting indexes %d-%d with bubble sort\n", datum.start, datum.end);
  int toSort[datum.size];
  memcpy(toSort, &data_array[datum.start], datum.size*sizeof(int));

  gettimeofday(&startt, NULL);
  BubbleSort(toSort, datum.size);
  gettimeofday(&stopt, NULL);

  datum.usecs_passed = stopt.tv_usec - startt.tv_usec;
  printf("Sorting indexes %d-%d with bubble sort in %li usecs\n", datum.start, datum.end, datum.usecs_passed);

  if(datum.usecs_passed > bubble_max){
    bubble_max = datum.usecs_passed;
  }

   if(datum.usecs_passed < bubble_min){
    bubble_min = datum.usecs_passed;
  }

  return NULL;
}

void* thread_merge(void* data){
  struct timeval startt, stopt;
  struct myData datum = *(struct myData*) data;
   printf("Sorting indexes %d-%d with merge sort\n", datum.start, datum.end);
     int toSort[datum.size];
    memcpy(toSort, &data_array[datum.start], datum.size*sizeof(int));

    gettimeofday(&startt, NULL);
    MergeSort(toSort, datum.size);
    gettimeofday(&stopt, NULL);

    datum.usecs_passed = stopt.tv_usec - startt.tv_usec;
    printf("Sorting indexes %d-%d with merge sort in %li usecs\n", datum.start, datum.end, datum.usecs_passed);

  if(datum.usecs_passed > merge_max){
    merge_max = datum.usecs_passed;
  }

  if(datum.usecs_passed < merge_min){
    merge_min = datum.usecs_passed;
  }

   return NULL;
}

int main(int argc, char** argv) {

    if(argc < 3) {
        printf("not enough arguments!\n");
        exit(1);
    }

    // I'm reading the value n (number of threads) for you off the
    // command line
    int n = atoi(argv[1]);
    if(n <= 0 || n > MAX_N_SIZE || n % 3 != 0) {
        printf("bad n value (number of threads) %d.  Must be a multiple of 3.\n", n);
        exit(1);
    }

    // I'm reading the number of values you want per thread
    // off the command line
    int vals_per_thread = atoi(argv[2]);
    if(vals_per_thread <= 0 || vals_per_thread > MAX_VALS_PER_THREAD) {
        printf("bad vals_per_thread value %d\n", vals_per_thread);
        exit(1);
    }

    int total_nums = n * vals_per_thread;

    data_array = malloc(sizeof(int) * total_nums);
    if(data_array == NULL) {
        perror("malloc failure");
        exit(1);
    }

    
    // initialize the test data for sort
    for(int i = 0; i < total_nums; i++) {
        // big reverse sorted list
        data_array[i] = total_nums - i;
        // the test would be more traditional if we used random
        // values, but this makes it easier for you to visually
        // inspect and ensure you're sorting everything
    }

    int start = 0;
    struct myData data[n];
    pthread_t threads[n];
    // create your threads here
    printf("N: %d\n", n);
    for(int i = 0; i < n; i++){
      // memcpy(arrays[i], &data_array[start], size*sizeof(int));
       data[i].start = start;
       data[i].end = start + vals_per_thread -1;
       data[i].size = vals_per_thread;

      int which = i % 3;
      data[i].sortMethod = which;
      switch (which)
      {
      case 0:
        pthread_create(&threads[i], NULL, thread_brute_force, &data[i]);
        break;
    
      case 1: 
        pthread_create(&threads[i], NULL, thread_bubble, &data[i]);
        break;
      case 2: 
        pthread_create(&threads[i], NULL, thread_merge, &data[i]);
        break;

      default:
        break;
      }

      
      start = start + vals_per_thread;
    }

    // wait for them to finish
    for(int i = 0; i < n; i++){ 
      pthread_join(threads[i], NULL);
    }

    // print out the algorithm summary statistics

    // print out the result array so you can see the sorting is working
    // you might want to comment this out if you're testing with large data sets
    printf("Result array:\n");
    for(int i = 0; i < n; i++) {
        for(int j = 0; j < vals_per_thread; j++) {
            printf("%d ", data_array[i*vals_per_thread + j]);
        }
        printf("\n");
    }

    suseconds_t brute_total = 0;
    suseconds_t bubble_total = 0;
    suseconds_t merge_total = 0;
    for(int i = 0; i < n; i++){
      struct myData myFunnyData = data[i];
     switch (myFunnyData.sortMethod)
      {
      case 0:
        brute_total = brute_total + myFunnyData.usecs_passed;
        break;
    
      case 1: 
       bubble_total = bubble_total + myFunnyData.usecs_passed;
        break;
      case 2: 
        merge_total = merge_total + myFunnyData.usecs_passed;
        break;

      default:
        break;
      }
    }

    printf("Merge total: %li\n", merge_total); 
    printf("Bub total: %li\n", bubble_total); 
    printf("Brute total: %li\n", brute_total); 

    printf("brute force avg %li min %li max %li\n", brute_total/3, brute_min, brute_max);
    printf("bubble avg %li min %li max %li\n", bubble_total/3, bubble_min, bubble_max);
    printf("merge avg %li min %li max %li\n", merge_total/3, merge_min, merge_max);

    free(data_array); // we should free what we malloc
}
