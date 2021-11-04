#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdbool.h>
#include <signal.h>
#include <sys/mman.h>
#include <unistd.h>
#include "forth/forth_embed.h"

// if the region requested is already mapped, things fail
// so we want address that won't get used as the program
// starts up
#define STACKHEAP_MEM_START 0xf9f8c000

// the number of memory pages will will allocate to an instance of forth
#define NUM_PAGES 20

// the max number of pages we want in memort at once, ideally
#define MAX_PAGES 3

int current_num_pages = 0;
int pages_with_files[NUM_PAGES] = {0};
int page_priorities[NUM_PAGES] = {0};
int files[NUM_PAGES] = {0};

static void handler(int sig, siginfo_t *si, void *unused)
{

    void *fault_address = si->si_addr;

    printf("in handler with invalid address %p\n", fault_address);
    int distance = (void *)fault_address - (void *)STACKHEAP_MEM_START;

    int pageNumber = distance / getpagesize();
    void *pageStart = (void *)STACKHEAP_MEM_START + pageNumber * getpagesize();

    // if ((distance < 0 || distance > getpagesize())
    // {
    //     printf("address not within expected page!\n");
    //     exit(2);
    // }

    //priority stuff
    for (int i = 0; i < NUM_PAGES; i++)
    {
        if (page_priorities[i] > 0)
        {
            page_priorities[i] = page_priorities[i] - 1;
        }
    }

    page_priorities[pageNumber] = MAX_PAGES;
    current_num_pages++;

    if (current_num_pages > MAX_PAGES)
    {
        //find lowest prioritiy
        int lowest = -1;
        for (int i = 0; i < NUM_PAGES; i++)
        {
            // printf("Page: %d, Priority: %d\n", i, page_priorities[i]);
            if (page_priorities[i] == 0)
            {
                lowest = i;
            }
        }
        //remove page
        int munmap_result = munmap((void *)STACKHEAP_MEM_START + lowest * getpagesize(), getpagesize());
        printf("unmapping page %d\n", lowest);
        page_priorities[lowest] = -1;
        if (munmap_result < 0)
        {
            perror("munmap failed");
            exit(6);
        }
        close(files[lowest]);
        current_num_pages--;
    }

    printf("mapping page %d\n", pageNumber);

    if (pages_with_files[pageNumber])
    {
        char buffer[200];
        sprintf(buffer, "page_%d.dat", pageNumber);
        int fd = open(buffer, O_RDWR | O_CREAT, S_IRWXU);
        mmap((void *)pageStart,
             getpagesize(),
             PROT_READ | PROT_WRITE | PROT_EXEC,
             MAP_FIXED | MAP_SHARED,
             fd,
             0);
        files[pageNumber] = fd;
    }
    else
    {
        pages_with_files[pageNumber] = 1;
        char buffer[200];
        sprintf(buffer, "page_%d.dat", pageNumber);
        int fd = open(buffer, O_RDWR | O_CREAT, S_IRWXU);
        char data = '\0';
        lseek(fd, getpagesize() - 1, SEEK_SET);
        write(fd, &data, 1);
        lseek(fd, 0, SEEK_SET);
        mmap((void *)pageStart,
             getpagesize(),
             PROT_READ | PROT_WRITE | PROT_EXEC,
             MAP_FIXED | MAP_SHARED,
             fd,
             0);
        files[pageNumber] = fd;
    }
}

int main()
{

    for (int i = 0; i < NUM_PAGES; i++)
    {
        page_priorities[i] = -1;
    }
    //TODO: Add a bunch of segmentation fault handler setup here for
    //PART 1 (plus you'll also have to add the handler your self)
    static char stack[SIGSTKSZ];
    stack_t ss = {
        .ss_size = SIGSTKSZ,
        .ss_sp = stack,
    };

    sigaltstack(&ss, NULL);
    struct sigaction sa;

    sa.sa_flags = SA_SIGINFO | SA_ONSTACK;
    sigemptyset(&sa.sa_mask);
    sa.sa_sigaction = handler;

    if (sigaction(SIGSEGV, &sa, NULL) == -1)
    {
        perror("error installing handler");
        exit(3);
    }

    struct forth_data forth;
    char output[200];

    // the return stack is a forth-specific data structure if we
    // wanted to, we could give it an expanding memory segment like we
    // do for the stack/heap but I opted to keep things simple
    int returnstack_size = getpagesize() * 2;
    void *returnstack = mmap(NULL, returnstack_size, PROT_READ | PROT_WRITE | PROT_EXEC,
                             MAP_ANON | MAP_PRIVATE, -1, 0);

    // initializing the stack/heap to a unmapped memory pointer we
    // will map it by responding to segvs as the forth code attempts
    // to read/write memory in that space

    int stackheap_size = getpagesize() * NUM_PAGES;

    // TODO: Modify this in PART 1
    void *stackheap = (void *)STACKHEAP_MEM_START;
    // void *stackheap = mmap(NULL, stackheap_size, PROT_READ | PROT_WRITE | PROT_EXEC,
    //                        MAP_ANON | MAP_PRIVATE, -1, 0);

    initialize_forth_data(&forth,
                          returnstack + returnstack_size, //beginning of returnstack
                          stackheap,                      //begining of heap
                          stackheap + stackheap_size);    //beginning of stack

    // this code actually executes a large amount of starter forth
    // code in jonesforth.f.  If you screwed something up about
    // memory, it's likely to fail here.
    load_starter_forth_at_path(&forth, "forth/jonesforth.f");

    printf("finished loading starter forth\n");

    // now we can set the input to our own little forth program
    // (as a string)
    int fresult = f_run(&forth,
                        " : USESTACK BEGIN DUP 1- DUP 0= UNTIL ; " // function that puts numbers 0 to n on the stack
                        " : DROPUNTIL BEGIN DUP ROT = UNTIL ; "    // funtion that pulls numbers off the stack till it finds target
                        " FOO 5000 USESTACK "                      // 5000 integers on the stack
                        " 2500 DROPUNTIL "                         // pull half off
                        " 1000 USESTACK "                          // then add some more back
                        " 4999 DROPUNTIL "                         // pull all but 2 off
                        " . . "                                    // 4999 and 5000 should be the only ones remaining, print them out
                        " .\" finished successfully \" "           // print some text */
                        ,
                        output,
                        sizeof(output));

    if (fresult != FCONTINUE_INPUT_DONE)
    {
        printf("forth did not finish executing sucessfully %d\n", fresult);
        exit(4);
    }
    printf("OUTPUT: %s\n", output);
    printf("done\n");
    return 0;
}
