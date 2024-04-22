CC?= gcc

CFLAGS = -std=gnu2x -Ofast -Wall -Wextra -pedantic -Wshadow -Wpointer-arith -Wcast-qual -Wstrict-prototypes -Wcast-qual -Wstrict-overflow=5 -Wno-unused-parameter -Werror -Wformat=2 -D_FORTIFY_SOURCE=1
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi -Iinclude
EXTRAFLAGS = -DDEBUG -ggdb

all: main.o Vulkan File shaders
	${CC} $(CFLAGS) $< Vulkan/vulkan.a -o HelloTriangle $(LDFLAGS) -LVulkan -l:vulkan.a -LVulkan/Error -l:error.a -LFile -l:file.a

.PHONY: test clean Vulkan Error File shaders

test: all
	./HelloTriangle

main.o: main.c
	${CC} -c ${CFLAGS} $< -o $@ ${LDFLAGS} ${EXTRAFLAGS}

%.o: %.c
	${CC} -c $< -o $@

rebuild: clean
	${MAKE} all

File:
	EXTRAFLAGS="${EXTRAFLAGS}" make -C File

Vulkan:
	EXTRAFLAGS="${EXTRAFLAGS}" make -C Vulkan

shaders:
	make -C shaders

clean:
	rm -f HelloTriangle *.o
	make -C Vulkan clean
	make -C File clean
	make -C shaders clean

run:
	./HelloTriangle
