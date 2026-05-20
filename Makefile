CC = gcc
CFLAGS = -Wall -pthread

all: parking

parking: main.c
	$(CC) $(CFLAGS) -o parking main.c

clean:
	rm -f parking
