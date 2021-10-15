#include "shell.h"


void
hibernate(void)
{
	if(fork() == 0){
		char* hibernate[1];
		hibernate[0] = "./hibernate"; 
		execlp(hibernate[0], hibernate[0], NULL);
	}else{
		wait(NULL);
	}
	
}

void
csse332_sleep(void)
{
	pid_t child = fork();
	if(child == 0){
		pid_t grandchild = fork();
		if(grandchild == 0){
			char* hibernate[1];
			hibernate[0] = "./hibernate"; 
			execlp(hibernate[0], hibernate[0], NULL);
		}else{
			wait(NULL);
			printf("Sleep process done in the background\n");
			exit(EXIT_SUCCESS);
		}
	}
}

void
cleanup(void)
{

}

void
pong(void)
{
	int me_pipe[2];
}


void
fpoint(void)
{
}

void
primes(void)
{
}

void
status(void)
{
	system("ps -a");
}

void
do_fp_work(void)
{
	double x, y;
	int a, b;

	srand(getpid());

	if(rand() >= RAND_MAX/2) {
		printf("PANIC\n");
		b = 0;
	} else {
		x = 10.0;
		y = 5.0;
		b = 1;
	}

	while(1) {
		x = x / ((a / b) * y);
	}
}

