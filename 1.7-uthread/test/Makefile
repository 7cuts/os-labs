CC       = cc
CFLAGS   = -g
LDFLAGS  = -L. -luthread
LIBS     = libuthread.so
BIN      = test_runner

TEST_SRCS := $(wildcard t/*.c)

# каталог, куда в итоге кладётся libuthread.so (после mv)
LIBDIR := $(CURDIR)

all: clean test

test: $(BIN)
	LD_LIBRARY_PATH=$(LIBDIR):$${LD_LIBRARY_PATH} ./$(BIN)

$(BIN): test.c $(TEST_SRCS) $(LIBS)
	$(CC) $(CFLAGS) test.c $(TEST_SRCS) -o $@ $(LDFLAGS)

$(LIBS):
	$(MAKE) -C ../lib
	mv ../lib/libuthread.so ./

clean:
	rm -f $(LIBS) $(BIN)
	$(MAKE) -C ../lib clean

.PHONY: all clean test
