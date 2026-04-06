.PHONY: all clean
CC = gcc
CFLAGS = -O2 -Wall -I ..
LDLIBS += -lcjson

all: adder

adder: adder.c ../csapp.o
	$(CC) $(CFLAGS) -o adder $^ $(LDLIBS)

clean:
	rm -f adder *~
