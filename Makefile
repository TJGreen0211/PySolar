C = x86_64-pc-cygwin-gcc
CFLAGS = -Wall
PROG = linearAlg

FILE_NAME = $(PROG)
MAKE_DIR = $(PWD)
SRCS = $(FILE_NAME).c
INCLUDE_PATHS = -I$(MAKE_DIR) -I/bin
LIBRARY_PATHS = -L/libs -lpthread

all : do_precomp target clean
.PHONY: all

test : do_precomp debug_compile
.PHONY: test

debug_compile : CFLAGS += -DDEBUG
debug_compile : target clean

do_precomp:
	$(CC) $(PROG).c -c
	
target: $(PROG)

$(PROG): $(SRCS)
	$(CC) -shared -o $(PROG).dll $(PROG).o
	#$(CC) $(CFLAGS) -o $(PROG) $(SRCS) $(INCLUDE_PATHS) $(LIBRARY_PATHS)

clean:
	rm -f $(PROG).o