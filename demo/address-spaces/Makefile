CFLAGS=-std=gnu99
CC=gcc

OS := $(shell uname)

ifeq ($(OS),Darwin)
	TARGETS=segfault break-stack break-heap
else
	TARGETS=memory-layout segfault break-stack break-heap casting test
endif

all: $(TARGETS)

clean:
	$(RM) $(TARGETS)
