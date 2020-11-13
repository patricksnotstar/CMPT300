CFLAGS = -g -Wall
OBJS = OSSim.o list.o

make: OSSim.o list.o
	gcc -o OSSim $(CFLAGS) $(OBJS)

main.o: main.c
	gcc -c OSSim.c

list.o: list.c list.h
	gcc -c list.c

clean:
	rm *.o OSSim
