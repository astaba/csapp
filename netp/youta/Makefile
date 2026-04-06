.PHONY: all clean cgi
CC = gcc
CFLAGS = -O2 -Wall -I .

# This flag includes the Pthreads library on a Linux box.
# Others systems will probably require something different.
LDLIBS += -lpthread

all: ytiny cgi

ytiny: ycsapp.o

cgi:
	$(MAKE) -C cgi-bin

clean:
	rm -rf *.o ytiny *~
	$(MAKE) -C cgi-bin clean
