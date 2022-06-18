cc = gcc
src_dir = src
test_dir = tests
third_dir = $(src_dir)/third

test_files = $(wildcard $(test_dir)/**/*.c)

all: build

server: force
	@$(cc) -pthread -o server $(src_dir)/web/server.c $(third_dir)/cJSON/libcjson.a $(third_dir)/libwebsockets/build/lib/libwebsockets.a
	@echo "Build complete"

server-debug: force
	@$(cc) -pthread -D DEBUG -o server $(src_dir)/web/server.c $(third_dir)/cJSON/libcjson.a $(third_dir)/libwebsockets/build/lib/libwebsockets.a
	@echo "Build complete"

build:
	@$(cc) -o main $(src_dir)/main.c $(third_dir)/cJSON/libcjson.a $(third_dir)/libwebsockets/build/lib/libwebsockets.a
	@echo "Build complete"

debug:
	@$(cc) -o main $(src_dir)/main.c -D DEBUG $(third_dir)/cJSON/libcjson.a $(third_dir)/libwebsockets/build/lib/libwebsockets.a
	@echo "Build complete"

mdebug:
	@$(cc) -o main $(src_dir)/main.c -fsanitize=address -D DEBUG
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

	rm -rf $(third_dir)/cJSON
	cd $(third_dir) && curl -L https://github.com/DaveGamble/cJSON/archive/master.zip -o cJSON.zip && unzip cJSON.zip && rm cJSON.zip && mv cJSON-master cJSON && cd cJSON && make && cd ../..

	rm -rf $(third_dir)/libwebsockets
	cd $(third_dir) && curl -L https://github.com/warmcat/libwebsockets/archive/main.zip -o libwebsockets.zip && unzip libwebsockets.zip && rm libwebsockets.zip && mv libwebsockets-main libwebsockets && cd libwebsockets && mkdir build && cd build && cmake .. -DLWS_WITH_SSL=OFF -DLWS_WITH_MINIMAL_EXAMPLES=OFF -DLWS_WITHOUT_CLIENT=ON -DLWS_WITHOUT_TESTAPPS=ON && make && make install && cd ../../..

	chmod -R 777 $(third_dir)

dockerfile: force
	docker buildx build --push --platform linux/arm64/v8,linux/amd64 --tag jacoblincool/bang-dev .

docker: force
	docker run --rm -it -p 8080:8080 jacoblincool/bang-dev bash -c '/app/server'

.PHONY: all build clean test force
