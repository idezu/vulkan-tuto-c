CC?=gcc

CFLAGS = 
CPPFLAGS = -I../include -I../

OBJ=instance.o extension.o device.o queue.o surface.o swapchain.o imageview.o graphicspipeline.o shadermodule.o \
 renderpass.o framebuffer.o commandpool.o commandbuffer.o sync.o buffer.o texture.o image.o objloader.o model.o

all: Error vulkan.a
	${AR} rcs vulkan.a ${OBJ} 
vulkan.a: ${OBJ}

image.o: image.c
	${CC} -c $(CFLAGS) $? -o $@ ${EXTRAFLAGS} -I../include
objloader.o: objloader.c
	${CC} -c $(CFLAGS) $? -o $@ ${EXTRAFLAGS} -I../include

%.o: %.c
	${CC} -c $(CFLAGS) $? -o $@ $(CPPFLAGS) ${EXTRAFLAGS} ${WARNINGS}

.PHONY: clean Error

Error:
	EXTRAFLAGS="${EXTRAFLAGS}" make -C Error

clean:
	rm -f *.o *.a
	make -C Error clean
