/* Copyright 2019 Rose-Hulman */
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

// number of carpenters
#define NUM_CARP 3
// number of painters
#define NUM_PAIN 3
// number of decorators
#define NUM_DECO 3

/**
  Imagine there is a shared memory space called house.

  There are 3 different kinds of operations on house: carpenters, painters, and
  decorators.  For any particular kind of operation, there can be an unlimited
  number of threads doing the same operation at once (e.g. unlimited carpenter
  threads etc.).  However, only one kind of operation can be done at a time (so
  even a single carpenter should block all painters and vice versa).

  Use mutex locks and condition variables to enforce this constraint.  You don't
  have to worry about starvation (e.g. that constantly arriving decorators might
  prevent carpenters from ever running) - though maybe it would be fun to
  consider how you would solve in that case.

  This is similar to the readers/writers problem BTW.
 **/

pthread_cond_t house;
pthread_mutex_t lock;

enum jobs
{
	idle = -1,
	carpentry = 0,
	painting = 1,
	decoration = 2
};

int current_job = idle;
int num_carps = 0;
int num_paints = 0;
int num_decs = 0;

void *carpenter(void *ignored)
{

	pthread_mutex_lock(&lock);
	if (current_job == idle)
	{
		current_job = carpentry;
	}

	while (current_job != carpentry)
	{
		pthread_cond_wait(&house, &lock);
		if (current_job == idle)
		{
			current_job = carpentry;
		}
	}

	current_job = carpentry;
	num_carps++;

	pthread_mutex_unlock(&lock);

	printf("starting carpentry\n");
	sleep(1);
	printf("finished carpentry\n");

	pthread_mutex_lock(&lock);
	num_carps--;
	if (num_carps <= 0)
	{
		current_job = idle;
		pthread_cond_broadcast(&house);
	}
	pthread_mutex_unlock(&lock);

	return NULL;
}

void *painter(void *ignored)
{

	pthread_mutex_lock(&lock);
	if (current_job == idle)
	{
		current_job = painting;
	}

	while (current_job != painting)
	{
		pthread_cond_wait(&house, &lock);
		if (current_job == idle)
		{
			current_job = painting;
		}
	}

	current_job = painting;
	num_paints++;

	pthread_mutex_unlock(&lock);

	printf("starting painting\n");
	sleep(1);
	printf("finished painting\n");

	pthread_mutex_lock(&lock);
	num_paints--;
	if (num_paints <= 0)
	{
		current_job = idle;
		pthread_cond_broadcast(&house);
	}
	pthread_mutex_unlock(&lock);

	return NULL;
}

void *decorator(void *ignored)
{
	pthread_mutex_lock(&lock);
	if (current_job == idle)
	{
		current_job = decoration;
	}

	while (current_job != decoration)
	{
		pthread_cond_wait(&house, &lock);
		if (current_job == idle)
		{
			current_job = decoration;
		}
	}

	current_job = decoration;
	num_decs++;

	pthread_mutex_unlock(&lock);

	printf("starting decorating\n");
	sleep(1);
	printf("finished decorating\n");

	pthread_mutex_lock(&lock);
	num_decs--;
	if (num_decs <= 0)
	{
		current_job = idle;
		pthread_cond_broadcast(&house);
	}
	pthread_mutex_unlock(&lock);
	return NULL;
}

int main(int argc, char **argv)
{
	pthread_t jobs[NUM_CARP + NUM_PAIN + NUM_DECO];
	for (int i = 0; i < NUM_CARP + NUM_PAIN + NUM_DECO; i++)
	{
		void *(*func)(void *) = NULL;
		if (i < NUM_CARP)
			func = carpenter;
		if (i >= NUM_CARP && i < NUM_CARP + NUM_PAIN)
			func = painter;
		if (i >= NUM_CARP + NUM_PAIN)
		{
			func = decorator;
		}
		pthread_create(&jobs[i], NULL, func, NULL);
	}

	for (int i = 0; i < NUM_CARP + NUM_PAIN + NUM_DECO; i++)
	{
		pthread_join(jobs[i], NULL);
	}

	printf("Everything finished.\n");
}
