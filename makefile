# Compiler flags
CC=gcc
CFLAGS=-std=c17 -Wall

# Files
OBJFILES=main.o server.o request.o

# Commands
ifeq ($(OS),Windows_NT)
MKDIR=if not exist $(1)\NUL mkdir $(1)
RM=del /Q /F
LFLAGS=-lws2_32
ifdef ComSpec
SHELL:=$(ComSpec)
endif
ifdef COMSPEC
SHELL:=$(COMSPEC)
endif
else
MKDIR=mkdir -p $(1)
RM=rm -rf
endif

all: release

release: CFLAGS += -O3
release: http clean

debug: CFLAGS += -ggdb
debug: http clean

http: $(OBJFILES)
	$(call MKDIR,build)
	$(CC) $(CFLAGS) -o build/http $(OBJFILES) $(LFLAGS)

main.o:
	$(CC) $(CFLAGS) -c src/main.c $(LFLAGS)

server.o:
	$(CC) $(CFLAGS) -c src/server.c $(LFLAGS)

request.o:
	$(CC) $(CFLAGS) -c src/request.c $(LFLAGS)

clean:
	$(RM) *.o
