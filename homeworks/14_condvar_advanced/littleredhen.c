/* Copyright 2016 Rose-Hulman */
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>

#define NUM_LOAVES_PER_BATCH 7
#define NUM_BATCHES 6
#define NUM_LOAVES_TO_EAT 14

/**
  This system has four threads: the duck, cat, and dog that eat 
  bread, and the little red hen that makes the bread. The little
  red hen makes seven loaves per batch, but she only has the patience
  for six batches. The little red hen only makes a batch if there are
  no loaves left.

  The other three animals each want to eat 14 loaves of
  bread, but only one of them can be in the kitchen at a time (to avoid
  fights over who gets what bread).

  When the duck, cat, or dog notices that there are no loaves of bread
  available, they complain to the little red hen and wait (in the kitchen)
  for the next batch to be ready.

  Use semaphores to enforce this constraint. Note: the global numLoaves 
  variable should be left as is (i.e. do not make it a semaphore).

  look at littleRedHenSampleOutput.txt for an example correct output
  sequence

 **/

int numLoaves;

pthread_mutex_t kitchen_lock;
pthread_cond_t waiting_on_bread;
pthread_cond_t waiting_to_bake;
pthread_cond_t in_the_kitchen;
bool someoneIsInTheKitchen = false;

void *littleRedHenThread(void *arg)
{
	char *name = (char *)arg;
	int batch;

	for (batch = 1; batch <= 6; batch++)
	{
		sleep(2); // just makes it obvious that it won't work without
		// semaphores

		pthread_mutex_lock(&kitchen_lock);
		while (numLoaves > 0)
		{
			pthread_cond_wait(&waiting_to_bake, &kitchen_lock);
		}

		numLoaves += 7;
		printf("%-20s: A fresh batch of bread is ready.\n", name);
		pthread_cond_broadcast(&waiting_on_bread);
		pthread_mutex_unlock(&kitchen_lock);
	}

	printf("%-20s: I'm fed up with feeding you lazy animals! "
		   "No more bread!\n",
		   name);
	return NULL;
}

void *otherAnimalThread(void *arg)
{
	char *name = (char *)arg;
	int numLoavesEaten = 0;

	while (numLoavesEaten < NUM_LOAVES_TO_EAT)
	{
		pthread_mutex_lock(&kitchen_lock);
		while (someoneIsInTheKitchen)
		{
			pthread_cond_wait(&in_the_kitchen, &kitchen_lock);
		}

		someoneIsInTheKitchen = true;
		// pthread_mutex_unlock(&kitchen_lock);

		while (numLoaves <= 0)
		{
			printf("%-20s: Hey, Little Red Hen, make some more bread!\n", name);
			pthread_cond_signal(&waiting_to_bake);
			pthread_cond_wait(&waiting_on_bread, &kitchen_lock);
		}

		// pthread_mutex_lock(&kitchen_lock);
		numLoaves--;

		printf("%-20s: Mmm, this loaf is delicious.\n", name);
		numLoavesEaten++;

		pthread_mutex_unlock(&kitchen_lock);

		pthread_mutex_lock(&kitchen_lock);

		someoneIsInTheKitchen = false;
		pthread_cond_signal(&in_the_kitchen);
		pthread_mutex_unlock(&kitchen_lock);

		if (random() > random())
		{ // Adds variety to output
			sleep(1);
		}
	}
	printf("%-20s: I've had my fill of bread. Thanks, Little Red Hen!\n", name);

	return NULL;
}

int main(int argc, char **argv)
{
	pthread_t dog, cat, duck, hen;
	numLoaves = 0;
	char dogName[] = "Lazy Dog";
	char catName[] = "Sleepy Cat";
	char duckName[] = "Noisy Yellow Duck";
	char henName[] = "Little Red Hen";

	pthread_create(&dog, NULL, otherAnimalThread, dogName);
	pthread_create(&cat, NULL, otherAnimalThread, catName);
	pthread_create(&duck, NULL, otherAnimalThread, duckName);
	pthread_create(&hen, NULL, littleRedHenThread, henName);
	pthread_join(dog, NULL);
	pthread_join(cat, NULL);
	pthread_join(duck, NULL);
	pthread_join(hen, NULL);

	printf("Everything finished.\n");
}
