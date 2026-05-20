CC = gcc
CFLAGS = -Wall -pthread
LDFLAGS = -lncurses

all: parking

# pour linstant on compile juste ce qui existe
parking: src/main.c src/voiture.c
	$(CC) $(CFLAGS) -o parking src/main.c src/voiture.c $(LDFLAGS)

clean:
	rm -f parking logs/*.log logs/*.csv
