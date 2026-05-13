CC = gcc
CFLAGS = -O3 -Wall -Wextra -pthread -std=c11
TARGET = msf.out
SRC = main.c, kruskal/kruskal.c
HDR = datatypes/datatypes.h, kruskal/kruscal.h

all: $(TARGET)

$(TARGET): $(SRC) $(HDR)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)

.PHONY: all clean

run: all
	./$(TARGET)