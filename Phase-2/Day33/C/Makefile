CC = gcc
SRC = TrieberStack.c

COMMON_FLAGS = -std=c11 -mcx16 -pthread -Wall -Wextra

DEBUG_FLAGS = -O1 -g

RELEASE_FLAGS = -O3

LDLIBS = -latomic

TSAN_FLAGS  = -fsanitize=thread,undefined
ASAN_FLAGS  = -fsanitize=address,undefined

.PHONY: all tsan asan release run-test run-bench clean

all: tsan

tsan: $(SRC)
	$(CC) $(COMMON_FLAGS) $(DEBUG_FLAGS) $(TSAN_FLAGS) $(SRC) -o stack_tsan $(LDLIBS)


asan: $(SRC)
	$(CC) $(COMMON_FLAGS) $(DEBUG_FLAGS) $(ASAN_FLAGS) $(SRC) -o stack_asan $(LDLIBS)

release: $(SRC)
	$(CC) $(COMMON_FLAGS) $(RELEASE_FLAGS) $(SRC) -o stack_release $(LDLIBS)

run-test: tsan asan
	setarch $$(uname -m) -R ./stack_tsan
	./stack_asan

run-bench: release
	./stack_release

clean:
	rm -f stack_tsan stack_asan stack_release