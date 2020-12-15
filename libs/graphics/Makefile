CC = gcc
CFLAGS = -Wall
PROG = graphics

MAKE_DIR = $(PWD)
SRCS = shader.c buffer.c framebuffer.c skybox.c
OBJS = shader.o buffer.o framebuffer.o skybox.o

INCLUDE_PATHS = -I C:/Users/wonde/OneDrive/Documents/Programming/PySolar/libs/GLFW
LIBRARY_PATHS = -L C:/Users/wonde/OneDrive/Documents/Programming/PySolar/libs

all : do_precomp target clean
.PHONY: all

test : debug_compile
.PHONY: test

debug_compile : CFLAGS += -DDEBUG
debug_compile : do_precomp target clean

do_precomp:
	$(CC) $(CFLAGS) -c $(SRCS)

target:
	ar rcs $(PROG).a $(OBJS)

clean:
	rm *.o