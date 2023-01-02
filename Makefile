CFLAGS=-std=c11 -g -static

cc: cc.c
		cc -o c/cc cc.c

test: c/cc
		./test.sh

clean:
		rm -f cc *.o *~~ tmp*

.PHONY: test cleanJJJ