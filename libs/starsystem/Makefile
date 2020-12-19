CC = gcc
CFLAGS = -Wall -Wno-unused-function
LIB_NAME=${PWD##*/}
PROG = starsystem

MAKE_DIR = $(PWD)
SRCS = starsystem.c planet.c waves/waves.c
OBJS = starsystem.o planet.o waves.o

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