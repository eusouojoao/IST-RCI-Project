TARGET=cot
CC=gcc
CFLAGS=-std=c99 -Wpedantic -Wconversion -Wall -Werror
LDFLAGS=-lm
HEADERS=$(wildcard hdr/*.h)
SRCS=$(wildcard src/*.c)
OBJS=$(addprefix bin/, $(notdir $(patsubst %.c, %.o, $(SRCS))))
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
all: $(shell mkdir -p bin) $(TARGET)

$(TARGET): $(OBJS) Makefile
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) -o $@

bin/%.o: src/%.c $(HEADERS) Makefile
	$(CC) -c $(CFLAGS) $< -o $@

.PHONY: clean
clean: ## -> opção CLEAN para limpar os object files e o executável; uso -> 'make clean'
	rm -rf bin/ $(TARGET)

.PHONY: debug
debug: ## -> opção DEBUG, acrescenta a flag de debug ao compilador; uso -> 'make debug'
debug: CFLAGS+=-ggdb -Wextra
debug: $(shell mkdir -p bin) $(TARGET)
