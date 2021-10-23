#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

pthread_cond_t wait_cond;
pthread_mutex_t mutex;
int in_there = 0;

void *thread(void *arg)
{
	char *letter = (char *)arg;
	printf("%c wants to enter the critical section\n", *letter);

	pthread_mutex_lock(&mutex);
	while(in_there >= 3)
	{
		pthread_cond_wait(&wait_cond, &mutex);
	}

	in_there++;

	printf("%c is in the critical section\n", *letter);
	sleep(1);
	printf("%c has left the critical section\n", *letter);
	
	in_there--;

	pthread_cond_signal(&wait_cond);
    pthread_mutex_unlock(&mutex);

	return NULL;
}

int main(int argc, char **argv)
{
	pthread_t threads[8];
	int i;
	char *letters = "abcdefgh";

	for (i = 0; i < 8; ++i)
	{
		pthread_create(&threads[i], NULL, thread, &letters[i]);

		if (i == 4)
			sleep(4);
	}

	for (i = 0; i < 8; i++)
	{
		pthread_join(threads[i], NULL);
	}

	printf("Everything finished...\n");

	return 0;
}
