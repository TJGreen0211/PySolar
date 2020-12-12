CC = gcc
CFLAGS = -Wall
PROG = matrixmath

MAKE_DIR = $(PWD)
SRCS = matrixMath.c
OBJS = matrixMath.o

INCLUDE_PATHS = -I C:/Users/wonde/OneDrive/Documents/Programming/PySolar/libs/GLFW
LIBRARY_PATHS = -L $(MAKE_DIR) -lm

all : do_precomp target clean
.PHONY: all

test : debug_compile
.PHONY: test

debug_compile : CFLAGS += -DDEBUG
debug_compile : do_precomp target clean

do_precomp:
	$(CC) $(CFLAGS) -c -o $(OBJS) $(SRCS) $(LIBRARY_PATHS)

target:
	ar rcs $(PROG).a $(OBJS)

clean:
	rm *.o