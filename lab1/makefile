CC = gcc
CFLAGS = -Wall -Wpedantic -g
LDFLAGS = -lm
OBJSRC = src/list.c
OBJTARGET = list.o
SRC = $(wildcard src/*.c)
TARGET = listTest.out
LIBTARGET = lib_list.a
DLIBFLAGS = -Wall -fPIC -D_GNU_SOURCE
DLIBTARGET = lib_list.so
TARGETS = $(TARGET) $(LIBTARGET) $(DLIBTARGET) $(OBJTARGET)

.PHONY: all clean

all: $(TARGETS) 

$(TARGETS): $(SRC)
	$(CC) $(CFLAGS) $(SRC) $(LDFLAGS) -o $(TARGET)
	$(CC) $(CFLAGS) -c $(OBJSRC) -o $(OBJTARGET)

	ar r $(LIBTARGET) $(OBJTARGET)
	ar tv $(LIBTARGET) $(OBJTARGET)

	$(CC) $(CFLAGS) -c $(DLIBFLAGS) $(OBJSRC) -o $(OBJTARGET)
	$(CC) $(OBJTARGET) -shared -o $(DLIBTARGET)

	cp $(LIBTARGET) ../lab2/src/$(LIBTARGET)
	cp $(DLIBTARGET) ../lab2/$(DLIBTARGET)

clean:
	rm -f $(TARGET)
	rm -f $(OBJTARGET)
	rm -f $(LIBTARGET)
	rm -f $(DLIBTARGET)
	rm -f ../lab2/$(DLIBTARGET)
	rm -f ../lab2/src/$(LIBTARGET)
	