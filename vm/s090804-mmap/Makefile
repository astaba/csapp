.PHONY: all clean

CC     := gcc
STD    := -std=gnu17
WAR    := -Wall -Werror
OPT    := -O3 -march=native -fdata-sections -ffunction-sections
CFLAGS := ${WAR}

SOURCES  = $(wildcard *.c)
BINARIES = $(subst .c,.out,$(SOURCES))

all: $(BINARIES)

$(BINARIES): %.out: %.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -fv *.out
