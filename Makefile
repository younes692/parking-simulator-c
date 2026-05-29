CC = gcc
CFLAGS = -Wall -pthread
LDFLAGS = -lncurses

all: parking

parking: src/main.c src/parking.c src/voiture.c src/logger.c
	$(CC) $(CFLAGS) -o parking src/main.c src/parking.c src/voiture.c \
		src/logger.c $(LDFLAGS)

clean:
	rm -f parking logs/*.log logs/*.csv
