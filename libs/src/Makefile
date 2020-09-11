CC = "C:/Program Files/mingw-w64/x86_64-8.1.0-posix-seh-rt_v6-rev0/mingw64/bin/gcc.exe"
CFLAGS = -Wall
PROG = geometry
#PROG = camera

FILE_NAME = $(PROG)
BIN_DIR = C:/Users/wonde/OneDrive/Documents/Programming/PySolar/bin
MAKE_DIR = $(PWD)
SRCS = pyGeometry.c sphere.c matrixMath.c quadcube.c
OBJS = pyGeometry.o sphere.o matrixMath.o quadcube.o
#SRCS = matrixMath.c pyCamera.c arcballCamera.c
#OBJS = matrixMath.o pyCamera.o arcballCamera.o

INCLUDE_PATHS = -I C:/Users/wonde/AppData/Local/Programs/Python/Python37/include
LIBRARY_PATHS = -L C:/Users/wonde/AppData/Local/Programs/Python/Python37/libs -lpython37

waves : all
.PHONY: waves

geometry : all
.PHONY: waves

waveinit:
	WAVE_SRCS = pyWaves.c waves.c matrixMath.c
	WAVE_OBJS = pyWaves.o waves.o matrixMath.o
	WAVE_INCLUDE_PATHS = -I C:/Users/wonde/AppData/Local/Programs/Python/Python37/include
	WAVE_LIBRARY_PATHS = -L C:/Users/wonde/AppData/Local/Programs/Python/Python37/libs -lpython37

all : do_precomp target
.PHONY: all

test : do_precomp debug_compile
.PHONY: test

debug_compile : CFLAGS += -DDEBUG
debug_compile : target clean

do_precomp:
	$(CC) $(CFLAGS) -c $(SRCS) $(INCLUDE_PATHS)

target:
	$(CC) -shared $(OBJS) $(LIBRARY_PATHS) -o $(PROG).pyd

#$(PROG): $(SRCS)
#gcc -Wall -c pyWaves.c waves.c matrixMath.c -I C:\Users\TJ\AppData\Local\Programs\Python\Python37\include
#gcc -shared interface.o water.o matrixMath.o -L C:\Users\TJ\AppData\Local\Programs\Python\Python37\libs -lpython37 -o waves.pyd

clean:
	mv C:/Users/wonde/OneDrive/Documents/Programming/PySolar/libs/src/$(PROG).pyd C:/Users/wonde/OneDrive/Documents/Programming/PySolar/bin/$(PROG).pyd
	rm -f $(OBJS)