CC=gcc
CFLAGS=-x c -std=c17

ifeq ($(OS),Windows_NT)
MKDIR=if not exist $(1)\NUL mkdir $(1)
RM = del /Q /F
LFLAGS=-lws2_32
ifdef ComSpec
SHELL := $(ComSpec)
endif
ifdef COMSPEC
SHELL := $(COMSPEC)
endif
else
MKDIR = mkdir -p
RM = rm -rf
endif

all: release

release:
	$(call MKDIR,build)
	$(CC) $(CFLAGS) -o2 src/main.c -o build/http $(LFLAGS)

debug:
	$(call MKDIR,debug)
	$(CC) $(CFLAGS) -ggdb src/main.c -o debug/http $(LFLAGS)

clean:
	$(RM) *.o
