CC?=gcc

CFLAGS = -std=gnu2x -Ofast -Wall -Wextra  -pedantic -Wshadow -Wpointer-arith -Wcast-qual -Wstrict-prototypes -Wcast-qual -Wstrict-overflow=5 -Wno-unused-parameter -Werror -Wformat=2 -D_FORTIFY_SOURCE=3
LDFLAGS = -I../include -I../

all: file.a
	${AR} rcs file.a  binary.o

file.a: binary.o


%.o: %.c
	${CC} -c $(CFLAGS) $? -o $@ $(LDFLAGS) ${EXTRAFLAGS}

.PHONY: clean

clean:
	rm -f *.o *.a
