# csapp/ch07-linking/s070601-duplicate-symbols/Makefile
# Created on: Wed Nov 19 12:42:08 +01 2025

.PHONY: all clean
.DELETE_ON_ERROR:

CC     = gcc
CFLAGS = -Wall -Og

# WARN: Define INDEX on the cmd line for all other binaries: INDEX=<no>
INDEX  = 1
SRC    = foo$(INDEX).c bar$(INDEX).c
BIN    = foo$(INDEX).out

all: $(BIN)

$(BIN): $(SRC)
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -rf *.out

