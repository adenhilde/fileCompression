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

testLZ: $(TARGET)
	rm -f $(OBJ)
	./$(TARGET) -zl lorem.txt lorem.txt.lz
	./$(TARGET) -ul lorem.txt.lz lorem_unzip_lz.txt
	@md5sum lorem.txt
	@md5sum lorem_unzip_lz.txt

testHUFF: $(TARGET)
	rm -f $(OBJ)
	./$(TARGET) -zh lorem.txt lorem.txt.huff
	./$(TARGET) -uh lorem.txt.huff lorem_unzip_huff.txt
	@md5sum lorem.txt
	@md5sum lorem_unzip_huff.txt

testALL: $(TARGET)
	rm -f $(OBJ)
	./$(TARGET) -z lorem.txt lorem.txt.bin
	./$(TARGET) -u lorem.txt.bin lorem_unzip_bin.txt
	@md5sum lorem.txt
	@md5sum lorem_unzip_bin.txt

clean:
	rm -f $(TARGET)
