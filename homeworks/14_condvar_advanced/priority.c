#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>

/**
  In this system there are threads numbered 1-6 and a critical
  section.  The thread numbers are priorities, where thread 6 is
  highest priority, thread 5 is next, etc.

  If the critical section is empty, any thread can enter.  While a
  thread is in the critical section no other threads can enter -
  regardless of priority. When a thread leaves the critical section
  and there are threads waiting, the highest priority waiting thread
  is allowed to enter the critical section.

  Be sure a newly arriving thread can't jump into the critical
  section as the current thread finishes, bypassing priority rules.
  Solve this problem with mutexes/condition variables
 **/

#define NUM_THREADS 6

pthread_mutex_t lock;
pthread_cond_t cond;

int priorities[NUM_THREADS];
int array_index = 0;
int threads_waiting = 0;

bool has_priority(int current)
{
	bool does_it_though = true;

	for (int i = 0; i < NUM_THREADS; i++){
		if(priorities[i] > current){
			does_it_though = false;
		}
	}

	return does_it_though;
}

void *thread(void *arg)
{
	int *num = (int *)arg;
	printf("%d wants to enter the critical section\n", *num);

	pthread_mutex_lock(&lock);
	threads_waiting++;
	priorities[array_index] = *num;
	int myIndex = array_index;
	array_index++;

	if (threads_waiting > 0)
	{
		while (!has_priority(*num))
		{
			pthread_cond_wait(&cond, &lock);
		}
	}

	pthread_mutex_unlock(&lock);

	printf("%d has entered the critical section\n", *num);
	sleep(1);
	printf("%d is finished with the critical section\n", *num);

	
	pthread_mutex_lock(&lock);
	threads_waiting--;
	priorities[myIndex] = 0;
	pthread_cond_broadcast(&cond);

	pthread_mutex_unlock(&lock);

	return NULL;
}

int main(int argc, char **argv)
{
	int i;
	pthread_t threads[6];
	int nums[] = {2, 1, 4, 3, 5, 6};

	for (i = 0; i < 6; i++)
	{
		pthread_create(&threads[i], NULL, thread, &nums[i]);

		if (i == 2)
			sleep(10);
	}

	for (i = 0; i < 6; i++)
	{
		pthread_join(threads[i], NULL);
	}

	printf("Everything finished.\n");
}


