# Compiler flags
CC=gcc
CFLAGS=-std=c17 -Wall

# Files
OBJFILES=build/main.o build/server.o build/request.o build/response.o build/print.o

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

debug: CFLAGS += -Og -ggdb
debug: http clean

http: $(OBJFILES)
	@$(call MKDIR,build)
	@echo Compiling executable...
	@$(CC) $(CFLAGS) -o build/http $(OBJFILES) $(LFLAGS)

build/%.o: src/%.c
	@echo Compiling $@
	@$(CC) $(CFLAGS) -c $< $(LFLAGS) -o $@

clean:
	@$(RM) build\*.o
