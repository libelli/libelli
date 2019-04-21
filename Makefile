SHELL=/bin/bash

SRC = $(wildcard src/*.c)
OBJ = ${SRC:.c=.o}
DEP = ${SRC:.c=.d}
EXE = libelli

INSTALL_DIR=/usr/local/bin

CC = clang
clang = $(shell which clang 2> /dev/null)
ifeq (, $(clang))
  CC = gcc
endif
C_FLAGS = -MMD -c -fPIC -Wall -Wpedantic -O3
C_LINKER_FLAGS = -lm


all: $(EXE)

$(EXE): $(OBJ)
	@$(CC) $^ $(C_LINKER_FLAGS) -o $@
	@printf ">> \e[1;36mLinking  \e[90m %s\e[0m\n" $@

-include $(OBJ:.o=.d)

%.o: %.c
	@$(CC) $(C_FLAGS) $< -o $@
	@printf ">> \e[1;32mCompiling\e[90m %s\e[0m\n" $@


.PHONY : clean
clean:
	@printf ">> \e[1;33mCleaning  \e[0m\n"
	@rm -f $(EXE) $(OBJ) $(DEP)

.PHONY : install
install: $(EXE) root-access
	@printf ">> \e[1;35mInstalling\e[0m\n"
	@cp $(EXE) $(INSTALL_DIR)

.PHONY : root-access
root-access:
	@if [[ $$UID != 0 ]]; then \
	  printf ">> \e[1;31mError\e[90m %s\e[0m\n" "Sudo permmision is required to install."; \
	  exit 1; \
	fi
