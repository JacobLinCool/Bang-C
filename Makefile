CC = gcc -I src/includes
SRC = src

all: build

build:
	@$(CC) -o main $(SRC)/main.c
	@echo "Build complete"

clean:
	@rm -f main
	@echo "Cleaned"

.PHONY: all build clean
