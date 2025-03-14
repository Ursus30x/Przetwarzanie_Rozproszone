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

.PHONY: all clean

all: $(TARGETDYNAMIC)

$(TARGETDYNAMIC): $(SRC)
	$(CC) $(CFLAGS) $(SRC) $(LIBSTATIC) $(LDFLAGS) -o $(TARGETSTATIC)

	$(CC) $(CFLAGS) -c $(SRC) -o $(OBJTARGETDYNAMIC)
	$(CC) $(CFLAGS) $(OBJTARGETDYNAMIC) -L. -l_$(LIBSDYNAMIC) -o $(TARGETDYNAMIC)
	
	

clean:
	rm -f $(TARGET)
	rm -f $(OBJTARGET)