TARGET=cot
CC=gcc
CFLAGS=-std=c99 -Wpedantic -Wconversion -Wall -Werror
LDFLAGS=-lm
SRCS=$(wildcard src/*.c)
OBJS=$(patsubst %.c, %.o, $(SRCS))
HEADERS=$(wildcard hdr/*.h)
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
all: $(TARGET)

$(TARGET): $(OBJS) Makefile
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) -o $@

%.o: %.c $(HEADERS) Makefile
	$(CC) -c $(CFLAGS) -o $@ $<

.PHONY: clean
clean: ## -> opção CLEAN para limpar os object files et al. (destroi o executável); uso -> 'make clean'
	rm -f ./src/*.o $(TARGET)

.PHONY: debug
debug: ## -> opção DEBUG, acrescenta a flag de debug ao compilador; uso -> 'make debug'
debug: CFLAGS+=-ggdb -Wextra
debug: $(TARGET)
