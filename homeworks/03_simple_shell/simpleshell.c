/* Copyright 2016 Rose-Hulman
   But based on idea from http://cnds.eecs.jacobs-university.de/courses/caoslab-2007/
   */
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

void myGoofyFunnyFunction(){
    wait(NULL);
}


int main() {
    char command[82];
    char *parsed_command[2];
    bool isBG = false;
    //takes at most two input arguments
    // infinite loop but ^C quits
    while (1) {
        printf("SHELL%% ");
        fgets(command, 82, stdin);
        command[strlen(command) - 1] = '\0';//remove the \n
        int len_1;
        for(len_1 = 0;command[len_1] != '\0';len_1++){
            if(command[len_1] == ' ')
                break;
        }

        parsed_command[0] = command;
        if(parsed_command[0][0] == 'B' && parsed_command[0][1] == 'G'){
            parsed_command[0] = parsed_command[0] + 2;
            isBG = true;
        }

        if(len_1 == strlen(command)){
            printf("Command is '%s' with no arguments\n", parsed_command[0]); 
            parsed_command[1] = NULL;
        }else{
            command[len_1] = '\0';
            parsed_command[1] = command + len_1 + 1;
            printf("Command is '%s' with argument '%s'\n", parsed_command[0], parsed_command[1]); 
        }


        if(isBG){
            pid_t chl1 = fork();
            if(chl1 == 0){
                pid_t chl2 = fork();
                if(chl2 == 0){
                    execlp(parsed_command[0], parsed_command[0], parsed_command[1], NULL);
                }else{
                    wait(NULL);
                    printf("Background command finished!\n");
                    exit(EXIT_SUCCESS);
                }
            }else{
                signal(SIGCHLD, myGoofyFunnyFunction);
            }
        }else{
            if(fork() == 0){
                execlp(parsed_command[0], parsed_command[0], parsed_command[1], NULL);
            }else{
                wait(NULL);
            }
        }
    }
}
