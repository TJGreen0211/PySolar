CC = gcc
CFLAGS = -Wall -Wno-unused-function
PROG = main

FILE_NAME = $(PROG)
BIN_DIR = C:/Users/wonde/OneDrive/Documents/Programming/PySolar/bin
MAKE_DIR = $(PWD)
SRCS = main.c \
	player.c \
	utility/quadtree/quadtree.c \
	glad/glad.c \
	matrixmath/matrixmath.a \
	starsystem/starsystem.a \
	camera/camera.a \
	graphics/graphics.a \
	geometry/geometry.a \
	simplexnoise/simplexnoise.a \
	audio/libportaudio.a
#OBJS = pyGeometry.o sphere.o matrixMath.o objLoader.o framebuffer.o player.o octree.o

INCLUDE_PATHS = -I $(MAKE_DIR)/GLFW
LIBRARY_PATHS = -L $(MAKE_DIR) -lglfw -lGL -lm -lrt -lasound -lpthread

all : do_precomp
.PHONY: all

test : debug_compile
.PHONY: test

debug_compile : CFLAGS += -DDEBUG
debug_compile : do_precomp target clean

do_precomp:
	@$(MAKE) -C camera
	#@$(MAKE) -C matrixmath
	@$(MAKE) -C geometry
	@$(MAKE) -C graphics
	@$(MAKE) -C simplexnoise
	@$(MAKE) -C starsystem
	$(CC) $(CFLAGS) -o $(PROG) $(SRCS) $(INCLUDE_PATHS) $(LIBRARY_PATHS)

target:
	$(CC) -shared $(OBJS) $(LIBRARY_PATHS) -o $(PROG).pyd

clean:
	move $(PROG).pyd ../../bin/
	del $(OBJS)