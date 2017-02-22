build:
	mkdir bin
	cc -O3 src/*.c -o bin/vm
	cc -O3 src/assembler/*.c -o bin/dis

test:
	lua tests/test.lua 2> /dev/null

debug:
	mkdir bin
	cc -D DEBUG src/*.c -o bin/vm
	cc -O3 src/assembler/*.c -o bin/dis
	lua tests/test.lua 2> debug.log

all: build test

clean:
	-rm bin/*
	-rmdir bin
	-rm debug.log
