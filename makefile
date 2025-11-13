CC = gcc
CFLAGS = -Wall -Wextra

TARGET = compress

SRC = compress.c huffman_tree.c huffman_codec.c LZ77.c
HEADERS = huffman_tree.h huffman_codec.h LZ77.h

OBJ = $(SRC:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)
