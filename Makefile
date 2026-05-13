CC = gcc
CFLAGS = -O3 -Wall -Wextra -pthread
TARGET = msf.out
SRC = main.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)

.PHONY: all clean
