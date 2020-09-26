CC = "C:/Program Files/mingw-w64/x86_64-8.1.0-posix-seh-rt_v6-rev0/mingw64/bin/gcc.exe"
CFLAGS = -Wall
PROG = geometry

FILE_NAME = $(PROG)
BIN_DIR = C:/Users/wonde/OneDrive/Documents/Programming/PySolar/bin
MAKE_DIR = $(PWD)
SRCS = ../src/matrixMath.c pyGeometry.c sphere.c quadcube.c objLoader.c
OBJS = pyGeometry.o sphere.o matrixMath.o quadcube.o objLoader.o

INCLUDE_PATHS = -I C:/Users/wonde/AppData/Local/Programs/Python/Python37/include
LIBRARY_PATHS = -L C:/Users/wonde/AppData/Local/Programs/Python/Python37/libs -lpython37

all : do_precomp target clean
.PHONY: all

test : debug_compile
.PHONY: test

debug_compile : CFLAGS += -DDEBUG
debug_compile : do_precomp target clean

do_precomp:
	$(CC) $(CFLAGS) -c $(SRCS) $(INCLUDE_PATHS)

target:
	$(CC) -shared $(OBJS) $(LIBRARY_PATHS) -o $(PROG).pyd

clean:
	move $(PROG).pyd ../../bin/
	del $(OBJS)