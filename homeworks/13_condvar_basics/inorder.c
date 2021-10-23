#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

int nextThread = 1;
pthread_cond_t wait_cond;
pthread_mutex_t mutex;

void *thread(void *arg)
{
        int *num = (int *)arg;
      
        printf("%d wants to enter the critical section\n", *num);

        pthread_mutex_lock(&mutex);
        while (nextThread != *num)
        {
                pthread_cond_wait(&wait_cond, &mutex);
        }


        
        nextThread++;
        printf("%d is finished with the critical section\n", *num);
        pthread_cond_signal(&wait_cond);
        pthread_mutex_unlock(&mutex);
        return NULL;
}

int main(int argc, char **argv)
{
        pthread_t threads[4];
        int i;
        int nums[] = {2, 1, 4, 3};

        for (i = 0; i < 4; ++i)
        {
                pthread_create(&threads[i], NULL, thread, &nums[i]);

                if (i == 2)
                        sleep(3);
        }

        for (i = 0; i < 4; ++i)
        {
                pthread_join(threads[i], NULL);
        }

        printf("Everything finished\n");

        return 0;
}