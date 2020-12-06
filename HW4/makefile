CFLAGS = -g -Wall
OBJS = UnixLs.o

make: UnixLs.o
	gcc -o UnixLs $(CFLAGS) $(OBJS)

main.o: main.c
	gcc -c UnixLs.c

clean:
	rm *.o UnixLs
