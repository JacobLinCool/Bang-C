SRC = src
cc = gcc

all: build

build:
	@$(CC) -o main $(SRC)/main.c
	@$(cc) -o main $(src_dir)/main.c
	@echo "Build complete"

clean:
	@rm -f main
	@echo "Cleaned"

.PHONY: all build clean
