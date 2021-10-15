all: shell hibernate ping

#################################
# PLEASE DO NOT DELETE THIS LINE
LDLIBS = -lreadline
#################################

.PHONY: all clean

clean:
	rm -f *.o shell hibernate ping

shell: functions.o shell.o
	gcc -o shell shell.o functions.o -lreadline

hibernate: hibernate.o functions.o
	gcc -o hibernate hibernate.o functions.o 

ping: ping.o
	gcc -o ping ping.o

shell.o: shell.c shell.h functions.c
	gcc -c shell.c shell.h functions.c 

hibernate.o: hibernate.c
	gcc -c hibernate.c

ping.o: ping.c
	gcc -c ping.c