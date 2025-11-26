# csapp/ch07-linking/s070100-compiler-driver/makefile
# Created on: Tue Nov 18 18:19:04 +01 2025

.PHONY: all clean
.DELETE_ON_ERROR:

CC     = gcc
CFLAGS = -Wall -Og
# CFLAGS = -Wall -g3 -O0 -DDEBUG
TEMP   = tmp/
SRC    = main.c sum.c
# This ensures the final link step depends on tmp/main.o and tmp/sum.o.
RLO = $(addprefix $(TEMP),$(subst .c,.o,$(SRC)))
BIN = $(subst .c,.out,$(word 1,$(SRC)))
# --- NEW DIRECTIVE ---
#  When compiling from intermediate files store in a "tmp/" directory make
#  will automatically delete all *.i and *.s file at the end of the compilation.
# .PRECIOUS tells make NOT to automatically delete files matching these
# patterns after they are used as intermediate dependencies in the build chain.
# .PRECIOUS: $(TEMP)%.i $(TEMP)%.s

all: $(BIN)

# Rule 1: Link the final binary (remains in root)
$(BIN): $(RLO)
	@echo "# ld Linker"
	$(CC) $(CFLAGS) $^ -o $@

# Rule 2: Compile Object Files (Target: tmp/%.o, Dependency: tmp/%.s)
$(TEMP)%.o: $(TEMP)%.s
	@echo "# as Assembler"
	$(CC) -c $< -o $@

# Rule 3: Generate Assembly Files (Target: tmp/%.s, Dependency: tmp/%.i)
$(TEMP)%.s: $(TEMP)%.i
	@echo "# cc1 Compiler"
	$(CC) -S $< -o $@

# Rule 4: Preprocess Files (Target: tmp/%.i, Dependency: %.c)
$(TEMP)%.i: %.c
	@mkdir -p ./$(TEMP)
	@echo "# cpp Preprocessor"
	$(CC) -E $< -o $@

clean:
	rm -rf *.out ./$(TEMP)
