# Create on: Tue Nov 25 16:15:51 +01 2025

.PHONY: all clean
.DELETE_ON_ERROR:

CC     = gcc
AR     = ar
CFLAGS = -Wall -Og

LIB     = lib/
TEMP0   = tmp/
TEMP1   = tmp_pic/
HEADER  = memlib.h

LIB_SRC = memlib.c mm.c
LIB_IDT = mm
BIN_SCR = test_mm.c

STLIB_RLO = $(addprefix $(TEMP0),$(subst .c,.o,$(LIB_SRC)))
STLIB_DIR = $(LIB)statlib/
STLIB_NAM = lib$(LIB_IDT).a
STLIB_PAT = $(STLIB_DIR)$(STLIB_NAM)

BIN_RLO = $(addprefix $(TEMP0),$(subst .c,.o,$(BIN_SCR)))
BIN_ST  = $(subst .c,_stat.out,$(BIN_SCR))

DLLIB_RLO = $(addprefix $(TEMP1),$(subst .c,.o,$(LIB_SRC)))
DLLIB_DIR = $(LIB)dynamlib/
DLLIB_NAM = lib$(LIB_IDT).so
DLLIB_PAT = $(DLLIB_DIR)$(DLLIB_NAM)

BIN_DL    = $(subst .c,_dyna.out,$(BIN_SCR))

all: $(BIN_ST) $(BIN_DL)

# WARNING:
# Object files that reference symbols must appears on the cmd line BEFORE
# the libraries that define the symbols.
# To test it reverse the order of dependencies in the next recipe.
# When linking from static libraries the order of relocatables objects
# (included static archive) matters. Relocatables with the most unresolved
# references set on the left while the most independent ones set on the right.

# Either way: this ... (could end with just $^)
$(BIN_ST): $(BIN_RLO) $(STLIB_PAT)
	$(CC) $(CFLAGS) -o $@ $^
	@tree .

# or that ...
# $(BIN_ST): $(BIN_RLO) $(STLIB_PAT)
# 	$(CC) $(CFLAGS) -o $@ $< -L./$(STLIB_DIR) -l$(LIB_IDT)

# This way is handy for development and testing or ...
$(BIN_DL): $(BIN_RLO) $(DLLIB_PAT)
	$(CC) $(CFLAGS) -o $@ $^
	@tree .

# This way update LD_LIBRARY_PATH before running executable
# $(BIN_DL): $(BIN_RLO) $(DLLIB_PAT)
# 	$(CC) $(CFLAGS) -o $@ $< -L./$(DLLIB_DIR) -l$(LIB_IDT)

$(STLIB_PAT): $(STLIB_RLO)
	@mkdir -p $(STLIB_DIR)
	$(AR) rcs $@ $^
	@echo ""
	@echo "--- Static Library Table Listing ---"
	$(AR) t $@
	@echo ""

$(DLLIB_PAT): $(DLLIB_RLO)
	@mkdir -p $(DLLIB_DIR)
	$(CC) $(CFLAGS) -shared $^ -o $@
	@echo ""

$(TEMP0)%.o: %.c $(HEADER)
	@mkdir -p $(TEMP0)
	$(CC) $(CFLAGS) -c $< -o $@

$(TEMP1)%.o: %.c $(HEADER)
	@mkdir -p $(TEMP1)
	$(CC) $(CFLAGS) -c -fpic $< -o $@

clean:
	rm -rf *.out $(TEMP0) $(TEMP1) $(LIB)
