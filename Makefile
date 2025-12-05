CC = gcc
CFLAGS = -Wall -O2 -pthread
LDFLAGS = -lrt -lpthread

all: server user

server: server.c common.h
	$(CC) server.c -o server $(CFLAGS) $(LDFLAGS)

user: user.c common.h
	$(CC) user.c -o user $(CFLAGS) $(LDFLAGS)

clean:
	rm -f server user
