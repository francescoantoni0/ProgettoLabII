CC = gcc
CFLAGS = -O3 -Wall -Wextra -pthread -std=c11
TARGET = msf.out
SRC = main.c
HDR = datatypes/datatypes.h

all: $(TARGET)

$(TARGET): $(SRC) $(HDR)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)

.PHONY: all clean

run: all
	./$(TARGET)