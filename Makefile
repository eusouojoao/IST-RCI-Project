TARGET=cot
CC=gcc
CFLAGS=-std=gnu99 -Wpedantic -Wconversion -Wall -Werror
LDFLAGS=
HEADERS=$(wildcard src/*/*.h) $(wildcard src/*/*/*.h)
SRCS=$(wildcard src/*.c) $(wildcard src/*/*.c) $(wildcard src/*/*/*.c)
OBJ_DIR=obj
OBJS=$(addprefix obj/, $(notdir $(patsubst %.c, %.o, $(SRCS))))

VPATH := $(sort $(dir $(SRCS)))

.DEFAULT_GOAL := all
.PHONY: help
help: ## -> invoca a lista de opções implementadas na Makefile; uso -> 'make help'
help:
	@grep -E '^[a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) \
	| sort \
	| awk 'BEGIN {FS = ":.*?## "}; {printf "\033[36m%-30s\033[0m %s\n", $$1, $$2}'

.PHONY: all
all: ## -> opção geral, i.e., quando se executa 'make' ou 'make all' no terminal
all: CFLAGS+=-O3
all: $(shell mkdir -p $(OBJ_DIR)) $(TARGET)

$(TARGET): $(OBJS) Makefile
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) -o $@

$(OBJ_DIR)/%.o: %.c $(HEADERS) Makefile
	$(CC) -c $(CFLAGS) $< -o $@

.PHONY: clean
clean: ## -> opção CLEAN para limpar os object files e o executável; uso -> 'make clean'
	rm -rf $(OBJ_DIR) $(TARGET) vgcore*

.PHONY: debug
debug: ## -> opção DEBUG, acrescenta a flag de debug ao compilador; uso -> 'make debug'
debug: CFLAGS+=-ggdb -Wextra
debug: $(shell mkdir -p $(OBJ_DIR)) $(TARGET)
