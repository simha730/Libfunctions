CC=gcc
CFLAGS=-Wall -Wextra -g

all: libkvstore.so server client

libkvstore.so: kvstore.c kvstore.h
	$(CC) -fPIC -shared kvstore.c -o libkvstore.so

server: server.c kvstore.h
	$(CC) server.c -o server -ldl

client: client.c
	$(CC) client.c -o client

clean:
	rm -f libkvstore.so server client

