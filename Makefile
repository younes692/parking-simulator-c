# compile le projet
all:
	gcc -Wall -pthread -o parking main.c

clean:
	rm -f parking
