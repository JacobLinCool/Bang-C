cc = gcc
src_dir = src
test_dir = tests
third_dir = $(src_dir)/third

uds_remote = https://raw.githubusercontent.com/JacobLinCool/Universal-Data-Structures/main/src
cimple_remote = https://raw.githubusercontent.com/JacobLinCool/Cimple-Lib/main/src

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

setup:
	rm -rf $(third_dir)
	mkdir $(third_dir)

	mkdir $(third_dir)/uds
	curl -s -o $(third_dir)/uds/vector.h $(uds_remote)/vector.h
	curl -s -o $(third_dir)/uds/xor-list.h $(uds_remote)/xor-list.h
	curl -s -o $(third_dir)/uds/deque.h $(uds_remote)/deque.h

	mkdir $(third_dir)/cimple
	curl -s -o $(third_dir)/cimple/all.h $(cimple_remote)/all.h
	curl -s -o $(third_dir)/cimple/base.h $(cimple_remote)/base.h
	curl -s -o $(third_dir)/cimple/buffer.h $(cimple_remote)/buffer.h
	curl -s -o $(third_dir)/cimple/bucket.h $(cimple_remote)/bucket.h
	curl -s -o $(third_dir)/cimple/console.h $(cimple_remote)/console.h
	curl -s -o $(third_dir)/cimple/debug.h $(cimple_remote)/debug.h
	curl -s -o $(third_dir)/cimple/file.h $(cimple_remote)/file.h
	curl -s -o $(third_dir)/cimple/string.h $(cimple_remote)/string.h
	curl -s -o $(third_dir)/cimple/timing.h $(cimple_remote)/timing.h
	curl -s -o $(third_dir)/cimple/options.h $(cimple_remote)/options.h

.PHONY: all build clean test force
