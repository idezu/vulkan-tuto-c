CC?= gcc

CFLAGS = -std=gnu2x -Og 
WARNINGS = -Wall -Wextra -pedantic -Wshadow -Wpointer-arith -Wcast-qual -Wstrict-prototypes -Wcast-qual -Wstrict-overflow=5 -Wno-unused-parameter -Werror -Wformat=2
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi -Iinclude
EXTRAFLAGS = -DDEBUG -ggdb -Og

all: main.o Vulkan File shaders
	${CC} $(CFLAGS) $< Vulkan/vulkan.a -o HelloTriangle $(LDFLAGS) -LVulkan -l:vulkan.a -LVulkan/Error -l:error.a -LFile -l:file.a -lm

.PHONY: test clean Vulkan Error File shaders

test: all
	./HelloTriangle

main.o: main.c
	${CC} -c ${CFLAGS} $< -o $@ ${LDFLAGS} ${EXTRAFLAGS} ${WARNINGS}

%.o: %.c
	${CC} -c $< -o $@

rebuild: clean
	${MAKE} all

File:
	EXTRAFLAGS="${EXTRAFLAGS}" WARNINGS="${WARNINGS}" make -C File

Vulkan:
	EXTRAFLAGS="${EXTRAFLAGS}" WARNINGS="${WARNINGS}" make -C Vulkan

shaders:
	make -C shaders

clean:
	rm -f HelloTriangle *.o
	make -C Vulkan clean
	make -C File clean
	make -C shaders clean

run:
	./HelloTriangle
