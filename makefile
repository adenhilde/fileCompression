CC = gcc
CFLAGS = -Wall -Wextra

TARGET = compress

SRC = src/compress.c src/huffman_tree.c src/huffman_codec.c src/LZ77.c
HEADERS = include/huffman_tree.h include/huffman_codec.h include/LZ77.h

OBJ = $(SRC:.c=.o)

all: $(TARGET)
	rm -f $(OBJ)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET)
