CC = gcc
CFLAGS = -Wall -Wextra -Iinclude -O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer
SRC = src/pager.c src/main.c
OBJ = $(SRC:.c=.o)
TARGET = pager_test

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	ASAN_OPTIONS=detect_leaks=1 ./$(TARGET)

clean:
	rm -f src/*.o $(TARGET) *.db

.PHONY: all run clean