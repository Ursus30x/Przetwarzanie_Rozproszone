CC = gcc
CFLAGS = -Wall -Wpedantic -g
LDFLAGS = 
SRC = $(wildcard src/*.c)
LIBSTATIC = $(wildcard src/*.a)
LIBSDYNAMIC= list
TARGETSTATIC = listAppStatic.out
OBJTARGETDYNAMIC = listAppDynamic.o
LIBTARGETDYNAMIC = listAppDynamic.so
TARGETDYNAMIC = listAppDynamic.out
TARGETS = $(TARGETSTATIC) $(OBJTARGETDYNAMIC) $(TARGETDYNAMIC)

.PHONY: all clean

all: $(TARGETS)

$(TARGETS): $(SRC)
	$(CC) $(CFLAGS) $(SRC) $(LIBSTATIC) $(LDFLAGS) -o $(TARGETSTATIC)

clean:
	rm -f $(TARGETSTATIC)
