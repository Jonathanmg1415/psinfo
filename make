CC=gcc
CFLAGS=-Wall -Iinclude
SRC=src/psinfo.c
BIN=build/psinfo

all: $(BIN)

$(BIN): $(SRC)
	mkdir -p build
	$(CC) $(CFLAGS) -o $(BIN) $(SRC)

clean:
	rm -rf build/*
	rm -f reports/*.info

.PHONY: all clean
