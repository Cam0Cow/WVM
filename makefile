build:
	cc -O3 src/*.c -o bin/vm
	cc -O3 src/assembler/*.c -o bin/dis

test:
	./tests/test.lua 2> /dev/null

debug:
	cc -D DEBUG src/*.c -o bin/vm
	./tests/test.lua 2> debug.log

all: build test

clean:
	-rm bin/*
	-rm debug.log
