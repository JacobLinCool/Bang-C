cc = gcc
src_dir = src
test_dir = tests
third_dir = $(src_dir)/third

test_files = $(wildcard $(test_dir)/**/*.c)

all: build

server: force
	@$(cc) -pthread -o server $(src_dir)/web/server.c $(third_dir)/wsServer/libws.a $(third_dir)/mkjson/lib/libmkjson.a $(third_dir)/cJSON/libcjson.a
	@echo "Build complete"

build:
	@$(cc) -o main $(src_dir)/main.c
	@echo "Build complete"

debug:
	@$(cc) -o main $(src_dir)/main.c -fsanitize=address -g -D DEBUG
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
	rm -rf $(third_dir)/uds
	cd $(third_dir) && curl -L https://github.com/JacobLinCool/Universal-Data-Structures/archive/main.zip -o uds.zip && unzip uds.zip && rm uds.zip && mv Universal-Data-Structures-main/src uds && rm -rf Universal-Data-Structures-main && cd ..

	rm -rf $(third_dir)/cimple
	cd $(third_dir) && curl -L https://github.com/JacobLinCool/Cimple-Lib/archive/main.zip -o cimple.zip && unzip cimple.zip && rm cimple.zip && mv Cimple-Lib-main/src cimple && rm -rf Cimple-Lib-main && cd ..

	rm -rf $(third_dir)/wsServer
	cd $(third_dir) && curl -L https://github.com/Theldus/wsServer/archive/master.zip -o wsServer.zip && unzip wsServer.zip && rm wsServer.zip && mv wsServer-master wsServer && cd wsServer && make && cd ../..

	rm -rf $(third_dir)/cJSON
	cd $(third_dir) && curl -L https://github.com/DaveGamble/cJSON/archive/master.zip -o cJSON.zip && unzip cJSON.zip && rm cJSON.zip && mv cJSON-master cJSON && cd cJSON && make && cd ../..

	chmod -R 777 $(third_dir)

.PHONY: all build clean test force
