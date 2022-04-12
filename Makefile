cc = gcc
src_dir = src
test_dir = tests

test_files = $(wildcard $(test_dir)/**/*.c)

all: build

build:
	@$(cc) -o main $(src_dir)/main.c
	@echo "Build complete"

test: $(test_files)
	@echo "Tests Passed"

$(test_files): force
	@echo "Running test: $(basename $@)"
	@$(cc) -o $(basename $@)_test $@
	@./$(basename $@)_test

clean:
	@rm -f main tests/**/*_test
	@echo "Cleaned"

.PHONY: all build clean test force
