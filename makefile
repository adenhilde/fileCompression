CC = gcc
CFLAGS = -Wall -Wextra

# binary name
TARGET = zipper

# source
SRC = zipper.c huffman_tree.c huffman_codec.c
HEADERS = huffman_tree.h huffman_codec.h

# create object files
OBJ = $(SRC:.c=.o)

# default rule
all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

# compile source files into objects
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

# clean build files
clean:
	rm -f $(OBJ) $(TARGET)
