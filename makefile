CC=gcc
CFLAGS=-x c -std=c17
LFLAGS=-lws2_32

all: http

http: 
	$(CC) $(CFLAGS) -o2 src/main.c -o http $(LFLAGS)

debug:
	$(CC) $(CFLAGS) -ggdb src/main.c -o http $(LFLAGS)

clean:
	rm -rf *.o
