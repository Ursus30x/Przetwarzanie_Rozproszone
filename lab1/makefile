CC = gcc
CFLAGS = -Wall -Wpedantic -g
LDFLAGS = -lm
SRC = $(wildcard src/*.c)
TARGET = listTest.out

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) $(LDFLAGS) -o $(TARGET)

clean:
	rm -f $(TARGET)