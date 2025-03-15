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

	$(CC) $(CFLAGS) -c $(SRC) -o $(OBJTARGETDYNAMIC)
	$(CC) $(CFLAGS) $(OBJTARGETDYNAMIC) -L. -l_$(LIBSDYNAMIC) -o $(TARGETDYNAMIC)
	
clean:
	rm -f $(TARGETSTATIC)
	rm -f $(OBJTARGETDYNAMIC)
	rm -f $(TARGETDYNAMIC)