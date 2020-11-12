CC = gcc
CFLAGS = -g -Wall
OBJS = s-talk.o inputModule.o list.o receiver.o screenModule.o sender.o

make: s-talk.o inputModule.o list.o receiver.o screenModule.o sender.o
	$(CC) -o s-talk $(CFLAGS) $(OBJS) -lpthread

s-talk.o: s-talk.c
	gcc -c s-talk.c

inputModule.o: inputModule.c inputModule.h
	gcc -c inputModule.c

list.o: list.c list.h
	gcc -c list.c

receiver.o: receiver.c receiver.h
	gcc -c receiver.c

screenModule.o: screenModule.c screenModule.h
	gcc -c screenModule.c

sender.o: sender.c sender.h
	gcc -c sender.c

clean:
	rm *.o s-talk
