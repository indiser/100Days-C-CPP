CXX = g++
# Fixed the spelling for your expected header, make sure your files match this.
SRC = TrieberStack.cpp

# C++20 is strictly required for std::jthread
COMMON_FLAGS = -std=c++20 -pthread -Wall -Wextra

# Optimization levels matter for sanitizers
DEBUG_FLAGS = -O1 -g
RELEASE_FLAGS = -O3

TSAN_FLAGS  = -fsanitize=thread
ASAN_FLAGS  = -fsanitize=address,undefined

.PHONY: all tsan asan release run-test run-bench clean

all: tsan

tsan: $(SRC)
	$(CXX) $(COMMON_FLAGS) $(DEBUG_FLAGS) $(TSAN_FLAGS) $(SRC) -o stack_tsan

asan: $(SRC)
	$(CXX) $(COMMON_FLAGS) $(DEBUG_FLAGS) $(ASAN_FLAGS) $(SRC) -o stack_asan

release: $(SRC)
	$(CXX) $(COMMON_FLAGS) $(RELEASE_FLAGS) $(SRC) -o stack_release

# WSL ASLR bypass remains critical for both sanitizers
run-test: tsan asan
	setarch $$(uname -m) -R ./stack_tsan
	setarch $$(uname -m) -R ./stack_asan

run-bench: release
	./stack_release

clean:
	rm -f stack_tsan stack_asan stack_release