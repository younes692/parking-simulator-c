CC = gcc
CFLAGS = -Wall -pthread
LDFLAGS = -lncurses

all: parking

parking: src/main.c src/parking.c src/voiture.c src/logger.c src/affichage.c src/stats.c
	$(CC) $(CFLAGS) -o parking src/main.c src/parking.c src/voiture.c \
		src/logger.c src/affichage.c src/stats.c $(LDFLAGS)

clean:
	rm -f parking logs/*.log logs/*.csv
