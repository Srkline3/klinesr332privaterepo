all: band littleredhen priority threeJobs

.PHONY: clean all

LDLIBS += -pthread

band: band.o

littleredhen: littleredhen.c
		gcc -pthread -o littleredhen littleredhen.c

priority: priority.c
		gcc -pthread -o priority priority.c

threeJobs: threeJobs.c
		gcc -pthread -o threeJobs threeJobs.c

clean:
		rm -rf *.o band littleredhen
csse@csse332:~/csse332/csse332_git/cs
