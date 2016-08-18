build:
	cc src/*.c -o bin/vm

test:
	./tests/test.lua

all: build test 
