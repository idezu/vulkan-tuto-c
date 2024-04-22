CC?=gcc

CFLAGS = -std=gnu2x -Ofast -Wall -Wextra  -pedantic -Wshadow -Wpointer-arith -Wcast-qual -Wstrict-prototypes -Wcast-qual -Wstrict-overflow=5 -Wno-unused-variable -Wno-unused-parameter -Werror -Wformat=2 -D_FORTIFY_SOURCE=3
LDFLAGS = -lglfw -lvulkan -I../include -I../

all: Error vulkan.a
	${AR} rcs vulkan.a  instance.o extension.o device.o queue.o surface.o swapchain.o imageview.o graphicspipeline.o shadermodule.o

vulkan.a: instance.o extension.o device.o queue.o surface.o swapchain.o imageview.o graphicspipeline.o shadermodule.o


%.o: %.c
	${CC} -c $(CFLAGS) $? -o $@ $(LDFLAGS) ${EXTRAFLAGS}

.PHONY: clean Error

Error:
	EXTRAFLAGS="${EXTRAFLAGS}" make -C Error

clean:
	rm -f *.o *.a
	make -C Error clean
