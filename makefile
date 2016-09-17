build:
	cc -O3 src/*.c -o bin/vm

test:
	./tests/test.lua 2> /dev/null

debug:
	cc -D DEBUG src/*.c -o bin/vm
	./tests/test.lua 2> debug.log

all: build test

clean:
	-rm bin/vm
	-rm debug.log
