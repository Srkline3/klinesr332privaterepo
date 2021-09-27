#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

int
main(int argc, char **argv)
{
        int fd, i;
        size_t nbytes;
        char buffer[1000];

        fd = open("/proc/csse332/status", O_RDONLY);
        if (fd < 0) {
                fprintf(stderr, "Could not open your the procfs entry for reading: %s\n",
                        strerror(errno));
                exit(EXIT_FAILURE);
        }

        for (i=0;i<12;++i) {
                nbytes = read(fd, buffer, 1000);
                if (nbytes > 0) {
                        printf("Read %lu bytes from procfs entry\n", nbytes);
                        printf("%s", buffer);
                } else {
                        printf("No more stuff to read at index %d\n", i);
                }
        }
}