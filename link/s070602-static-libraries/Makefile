# csapp/ch07-linking/s070602-static-libraries/Makefile
# Created on: Wed Nov 19 13:54:42 +01 2025

.PHONY: all clean
.DELETE_ON_ERROR:

CC     = gcc
AR     = ar
CFLAGS = -Wall -Og

TEMP   = tmp/
HEADER = vector.h

STLIB_SRC = addvec.c multvec.c
STLIB_RLO = $(addprefix $(TEMP),$(subst .c,.o,$(STLIB_SRC)))
STLIB_DIR = statlib/
STLIB_IDT = vector
STLIB_NAM = lib$(STLIB_IDT).a
STLIB_PAT = $(STLIB_DIR)$(STLIB_NAM)

BIN_SCR = main2.c
BIN_RLO = $(addprefix $(TEMP),$(subst .c,.o,$(BIN_SCR)))
BIN     = $(subst .c,.out,$(BIN_SCR))

all: $(BIN)

# WARNING:
# Object files that reference symbols must appears on the cmd line BEFORE
# the libraries that define the symbols. This following would fail:
# gcc -static ./statlib/libvector.a ./main2.c
# TEST: To test it reverse the order of dependencies in the next recipe.
# NOTE: When linking from static libraries the order of relocatables objects
# (included static archive) matters. Relocatables with the most unresolved
# references set on the left while the most independent ones set on the right.

# Either way: this ... (could end with just $^)
$(BIN): $(BIN_RLO) $(STLIB_PAT)
	$(CC) $(CFLAGS) -o $@ tmp/main2.o statlib/libvector.a

# or that ...
# $(BIN): $(BIN_RLO) $(STLIB_PAT)
# 	$(CC) $(CFLAGS) -o $@ $< -L./$(STLIB_DIR) -l$(STLIB_IDT)

$(STLIB_PAT): $(STLIB_RLO)
	@mkdir -p $(STLIB_DIR)
	$(AR) rcs $@ $^
	@tree .
	@echo ""
	@echo "--- Static Library Table Listing ---"
	$(AR) t $@
	@echo ""

$(TEMP)%.o: %.c $(HEADER)
	@mkdir -p $(TEMP)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf *.out $(TEMP) $(STLIB_DIR)

