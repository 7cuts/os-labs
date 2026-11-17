SOURCES = sched.c self.c thread_list.c create.c join.c log.c

CC=gcc

all: libuthread

CFLAGS= -shared -o libuthread.so -fPIC -g

clean:
	rm -f libuthread.so

libuthread: $(SOURCES)
	$(CC) $(CFLAGS) $(SOURCES)


.PHONY: all
