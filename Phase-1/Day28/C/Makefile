CC = gcc
CFLAGS_COMMON = -Wall -Wextra -std=c11
CFLAGS_DEBUG = $(CFLAGS_COMMON) -g -O0 -fsanitize=undefined,alignment -fno-sanitize-recover=all
CFLAGS_RELEASE = $(CFLAGS_COMMON) -O3

SRC = ub_catalog.c
TARGET_DEBUG = ub_debug
TARGET_RELEASE = ub_release

.PHONY: all debug release clean run-debug run-release

all: debug release

debug: $(SRC)
	$(CC) $(CFLAGS_DEBUG) $(SRC) -o $(TARGET_DEBUG)

release: $(SRC)
	$(CC) $(CFLAGS_RELEASE) $(SRC) -o $(TARGET_RELEASE)

clean:
	rm -f $(TARGET_DEBUG) $(TARGET_RELEASE)

run-debug: debug
	@echo "=== RUNNING ALL DEBUG (UBSAN) CASES ==="
	-./$(TARGET_DEBUG) 1
	-./$(TARGET_DEBUG) 2
	-./$(TARGET_DEBUG) 3
	-./$(TARGET_DEBUG) 4
	-./$(TARGET_DEBUG) 5

run-release: release
	@echo "=== RUNNING ALL RELEASE (-O3) CASES ==="
	-./$(TARGET_RELEASE) 1
	-./$(TARGET_RELEASE) 2
	-./$(TARGET_RELEASE) 3
	-./$(TARGET_RELEASE) 4
	-./$(TARGET_RELEASE) 5