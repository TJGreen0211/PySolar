CC = "C:/Program Files/mingw-w64/x86_64-8.1.0-posix-seh-rt_v6-rev0/mingw64/bin/gcc.exe"
CFLAGS = -Wall
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
	del *.o