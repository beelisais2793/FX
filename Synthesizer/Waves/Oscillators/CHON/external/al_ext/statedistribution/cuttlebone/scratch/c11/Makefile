.SUFFIXES:
.PHONY: _

_: queue

queue: main.o queue.o
	cc -std=c11 -o $@ $^

%.o : %.c
	cc -std=c11 -o $@ -c $<

clean:
	@rm -rf queue main.o queue.o
