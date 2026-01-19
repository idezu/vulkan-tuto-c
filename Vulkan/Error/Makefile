CC?=gcc

CFLAGS = -std=gnu2x -Ofast -Wall -Wextra  -pedantic -Wshadow -Wpointer-arith -Wcast-qual -Wstrict-prototypes -Wcast-qual -Wstrict-overflow=5  -Werror -Wno-unused-parameter -Wformat=2
LDFLAGS = -lvulkan -I../../include -I../../

all: error.o | error.a

error.a: error.o
	${AR} rcs $@ $?

error.o: error.c
	${CC} -c $(CFLAGS) $? -o $@ $(LDFLAGS) ${EXTRAFLAGS}

.PHONY: clean

clean:
	rm -f *.o *.a
