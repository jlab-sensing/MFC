INCLUDES = -I./external/libipc/include
CFLAGS = -Wall -Wextra -g $(INCLUDES)
LDFLAGS = -L./external/libipc/build
LDLIBS = -lipc

OBJS := teroslogger.o

# Add build prefix
OBJS := $(addprefix build/,$(OBJS))

.PHONY: all
all: libipc build/teroslogger

build/teroslogger: $(OBJS)
	$(CC) $(LDFLAGS) $^ $(LOADLIBES) $(LDLIBS) -o $@

.PHONY: libipc
libipc: external/libipc/Makefile
	cd external/libipc/ && make

build/%.o: src/%.c
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $< -o $@

.PHONY: clean
clean:
	rm -rf build/*
	cd external/libipc/ && make clean
