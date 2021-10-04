CFLAGS=-g
CC=gcc

LIBS= -lssl -lcrypto

PROG = server.out client.out

all: $(PROG)

server.out: encrypt.o server.c
	$(CC) $(CFLAGS) -o server.out encrypt.o server.c $(LIBS)

client.out: encrypt.o client.c
	$(CC) $(CFLAGS) -o client.out encrypt.o client.c $(LIBS)

encrypt.o: encrypt.h encrypt.c
	$(CC) $(CFLAGS) -c encrypt.h encrypt.c $(LIBS)

clean:
	rm $(PROG) *.o *.gch