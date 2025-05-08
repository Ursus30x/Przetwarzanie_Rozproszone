CC = gcc
CFLAGS = -Wall -Wpedantic -g
LDFLAGS = -lpthread
SRC = $(wildcard src/*.c)
LIBSTATIC = $(wildcard src/*.a)
LIBSDYNAMIC= list
TARGETSTATIC = program.out

TARGETS = $(TARGETSTATIC)

.PHONY: all clean

all: $(TARGETS)

$(TARGETS): $(SRC)
	$(CC) $(CFLAGS) $(SRC)  $(LDFLAGS) -o $(TARGETSTATIC)

clean:
	rm -f $(TARGETSTATIC)
