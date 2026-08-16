CC = gcc
CFLAGS = -O3 -Wall -Wextra -Werror -std=c11 -D_POSIX_C_SOURCE=199309L
LDFLAGS = -lm

SRC = main.c benchmark.c
OBJ = $(SRC:.c=.o)
TARGET = bench_c

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Automatically pin core 0 to suppress migration noise
run: $(TARGET)
	@echo "Note: Running pinned to CPU core 0. For best results disable CPU turbo boost."
	taskset -c 0 ./$(TARGET)

clean:
	rm -f $(OBJ) $(TARGET)