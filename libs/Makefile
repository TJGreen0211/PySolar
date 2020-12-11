CC = "C:/Program Files/mingw-w64/x86_64-8.1.0-posix-seh-rt_v6-rev0/mingw64/bin/gcc.exe"
CFLAGS = -Wall
PROG = main

FILE_NAME = $(PROG)
BIN_DIR = C:/Users/wonde/OneDrive/Documents/Programming/PySolar/bin
MAKE_DIR = $(PWD)
SRCS = main.c \
	glad/glad.c \
	starsystem/starsystem.a \
	graphics/graphics.a \
	matrixmath/matrixmath.a \
	simplexnoise/simplexnoise.a \
	camera/camera.a \
	player.c
OBJS = pyGeometry.o sphere.o matrixMath.o objLoader.o framebuffer.o player.o octree.o

INCLUDE_PATHS = -I C:/Users/wonde/OneDrive/Documents/Programming/PySolar/libs/GLFW
LIBRARY_PATHS = -L C:/Users/wonde/OneDrive/Documents/Programming/PySolar/libs -lglfw3 -lopengl32 -lgeometry

all : do_precomp
.PHONY: all

test : debug_compile
.PHONY: test

debug_compile : CFLAGS += -DDEBUG
debug_compile : do_precomp target clean

do_precomp:
	@$(MAKE) -C camera
	@$(MAKE) -C graphics
	@$(MAKE) -C starsystem
	$(CC) $(CFLAGS) -o $(PROG) $(SRCS) $(INCLUDE_PATHS) $(LIBRARY_PATHS)

target:
	$(CC) -shared $(OBJS) $(LIBRARY_PATHS) -o $(PROG).pyd

clean:
	move $(PROG).pyd ../../bin/
	del $(OBJS)