CC = gcc
CFLAGS = -Wall -Wextra -Werror -Iinclude

all: build

build:
	mkdir -p build
	$(CC) src/*.c $(CFLAGS) -o build/program

clean:
	rm -rf build

.PHONY: all build clean