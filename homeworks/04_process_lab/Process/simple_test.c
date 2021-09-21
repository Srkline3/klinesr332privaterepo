#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/wait.h>
#include<string.h>

#define simple_assert(message, test) do { if (!(test)) return message; } while (0)
#define TEST_PASSED NULL
#define DATA_SIZE 100
#define INITIAL_VALUE 77
#define MAX_TESTS 10

char* (*test_funcs[MAX_TESTS])(); // array of function pointers that store
                           // all of the tests we want to run
int num_tests = 0;

int data[DATA_SIZE][DATA_SIZE]; // shared data that the tests use

void timeout(){
    exit(2);
}

void add_test(char* (*test_func)()) {
    if(num_tests == MAX_TESTS) {
        printf("exceeded max possible tests");
        exit(1);
    }
    test_funcs[num_tests] = test_func;
    num_tests++;
}
// this setup function should run before each test
void setup() {
    printf("starting setup\n");
    for(int i = 0; i < DATA_SIZE; i++) {
        for(int j = 0; j < DATA_SIZE; j++) {
            data[i][j] = INITIAL_VALUE;
        }
    }
    // imagine this function does a lot of other complicated setup
    // that takes a long time
    usleep(3000000);
    
}

char* test1();

void* run_test(void *test_to_run_void) {
    setup();
    char* (*test_func)() = test_to_run_void;
    return test_func();
}

void run_all_tests() {

    int processes[num_tests];
    setup();
    int fd[2];
    pipe(fd);

    for(int i = 0; i < num_tests; i++){
        processes[i] = fork();
        if(processes[i] == 0){
            signal(SIGALRM, timeout);
            alarm(3);
            char* result = test_funcs[i]();
            if(result == TEST_PASSED){
                exit(EXIT_SUCCESS);
                close(fd[1]);
            }else{
                write(fd[1], result, strlen(result));
                close(fd[1]);
                exit(EXIT_FAILURE);
            }
        }
    }

    for(int j = 0; j < num_tests; j++){
        int status;
        waitpid(processes[j], &status, 0);

        if(WIFEXITED(status)){
            if(WEXITSTATUS(status) == 1){
                char error[80];
                read(fd[0], error, 80);
                printf("Test Failed: %s :(\n", error);
            } else if(WEXITSTATUS(status) == 2){
                printf("Test timed out 0_0\n");
            } else{
                printf("Test passed :^)\n");
            }
        }else{
            printf("Test Crashed w_w\n");
        } 
    }        
}

char* test1() {

    printf("starting test 1\n");
    for(int i = 0; i < DATA_SIZE; i++) {
        for(int j = 0; j < DATA_SIZE; j++) {
            simple_assert("test 1 data not initialized properly", data[i][j] == INITIAL_VALUE);
        }
    }

    for(int i = 0; i < DATA_SIZE; i++) {
        for(int j = 0; j < DATA_SIZE; j++) {
            data[i][j] = 1;
        }
    }
    
    for(int i = 0; i < DATA_SIZE; i++) {
        for(int j = 0; j < DATA_SIZE; j++) {
            simple_assert("test 1 data not set properly", data[i][j] == 1);
        }
    }
    printf("ending test 1\n");
    return TEST_PASSED;
}

char* test2() {

    printf("starting test 2\n");
    for(int i = 0; i < DATA_SIZE; i++) {
        for(int j = 0; j < DATA_SIZE; j++) {
            simple_assert("test 2 data not initialized properly", data[i][j] == INITIAL_VALUE);
        }
    }

    for(int i = 0; i < DATA_SIZE; i++) {
        for(int j = 0; j < DATA_SIZE; j++) {
            data[i][j] = 2;
        }
    }

    for(int i = 0; i < DATA_SIZE; i++) {
        for(int j = 0; j < DATA_SIZE; j++) {
            simple_assert("test 2 data not set properly", data[i][j] == 2);
        }
    }

    printf("ending test 2\n");
    return TEST_PASSED;
}

char* test3() {

    printf("starting test 3\n");

    simple_assert("test 3 always fails", 1 == 2);
    
    printf("ending test 3\n");
    return TEST_PASSED;
}


char* test4() {

    printf("starting test 4\n");

    int *val = NULL;
    printf("data at val is %d", *val);
    
    printf("ending test 4\n");
    return TEST_PASSED;
}

char* test5() {

    printf("starting test 5\n");

    while(1) { } 
    
    printf("ending test 5\n");
    return TEST_PASSED;
}

void main() {
    add_test(test1);
    add_test(test2);
    add_test(test3);
    add_test(test4); // uncomment for Step 4
    add_test(test5); // uncomment for Step 5
    run_all_tests();
    
}
