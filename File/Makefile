CC?=gcc

CFLAGS = -D_FORTIFY_SOURCE=3
CPPFLAGS = -I../include -I../

all: file.a
	${AR} rcs file.a  binary.o

file.a: binary.o


%.o: %.c
	${CC} -c $(CFLAGS) ${CPPFLAGS} $? -o $@ $(LDFLAGS) ${EXTRAFLAGS} ${WARNINGS}

.PHONY: clean

clean:
	rm -f *.o *.a
