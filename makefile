# Compiler flags
CC=clang
CFLAGS=-Wall -pedantic

# Files
OBJFILES=build/main.o build/server.o build/request.o build/response.o build/print.o

all: release

release: CFLAGS += -O3
release: http

debug: CFLAGS += -Og -ggdb
debug: http

http: $(OBJFILES)
	@mkdir -p build
	@echo Compiling executable...
	@$(CC) $(CFLAGS) -o build/http $(OBJFILES)

build/%.o: src/%.c
	@echo Compiling $@...
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -f build/*.o

.PHONY: http clean
