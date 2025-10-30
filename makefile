CC = gcc
CFLAGS = -Wall -Wextra

# binary name
TARGET = zipper

# source
SRC = zipper.c file_io.c
HEADERS = file_io.h 

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
