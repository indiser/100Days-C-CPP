CC = gcc
CFLAGS = -Wall -Wextra -O2 -std=c11
TARGET = epoll_server
SRC = async.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $@ $^

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: all run clean