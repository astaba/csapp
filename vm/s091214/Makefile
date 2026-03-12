# csapp/ch09-virtual-memory/s091214/Makefile
# Created on: Mon Dec  1 18:34:31 +01 2025

.PHONY: all clean test

CC     := gcc
CFLAGS := -Wall -Werror

SRC = $(wildcard *.c)
BIN = $(subst .c,,$(SRC))

all: $(BIN)

$(BIN): %: %.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -fv $(BIN) *.out
