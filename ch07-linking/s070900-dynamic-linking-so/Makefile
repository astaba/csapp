# csapp/ch07-linking/s070900-dynamic-linking-so/Makefile
# Created on: Sat Nov 22 22:32:44 +01 2025

.PHONY: all clean
.DELETE_ON_ERROR:

CC     = gcc
AR     = ar
CFLAGS = -Wall -Og

TEMP   = tmp/
HEADER = vector.h

DLLIB_SRC = addvec.c multvec.c
DLLIB_RLO = $(addprefix $(TEMP),$(subst .c,.o,$(DLLIB_SRC)))
DLLIB_DIR = dynamlib/
DLLIB_IDT = vector
DLLIB_NAM = lib$(DLLIB_IDT).so
DLLIB_PAT = $(DLLIB_DIR)$(DLLIB_NAM)

BIN_SCR = main2.c
BIN_RLO = $(addprefix $(TEMP),$(subst .c,.o,$(BIN_SCR)))
BIN     = $(subst .c,.out,$(BIN_SCR))

all: $(BIN)

$(BIN): $(BIN_RLO) $(DLLIB_PAT)
	$(CC) $(CFLAGS) -o $@ $^

$(DLLIB_PAT): $(DLLIB_RLO)
	@mkdir -p $(DLLIB_DIR)
	$(CC) $(CFLAGS) -shared $^ -o $@
	@tree .
	@echo ""

$(TEMP)%vec.o: %vec.c $(HEADER)
	@mkdir -p $(TEMP)
	$(CC) $(CFLAGS) -c -fpic $< -o $@

$(TEMP)%2.o: %2.c $(HEADER)
	@mkdir -p $(TEMP)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf *.out $(TEMP) $(DLLIB_DIR)

