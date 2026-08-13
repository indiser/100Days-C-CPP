CXX = g++
CXXFLAGS_COMMON = -Wall -Wextra -std=c++20
CXXFLAGS_DEBUG = $(CXXFLAGS_COMMON) -g -O0 -fsanitize=undefined,address -fno-sanitize-recover=all
CXXFLAGS_RELEASE = $(CXXFLAGS_COMMON) -O3

SRC = ub_cpp_catalog.cpp
TARGET_DEBUG = ub_cpp_debug
TARGET_RELEASE = ub_cpp_release

.PHONY: all debug release clean run-debug run-release

all: debug release

debug: $(SRC)
	$(CXX) $(CXXFLAGS_DEBUG) $(SRC) -o $(TARGET_DEBUG)

release: $(SRC)
	$(CXX) $(CXXFLAGS_RELEASE) $(SRC) -o $(TARGET_RELEASE)

clean:
	rm -f $(TARGET_DEBUG) $(TARGET_RELEASE)

run-debug: debug
	@echo "=== C++ DEBUG (UBSan + ASan) ==="
	-./$(TARGET_DEBUG) 1
	-./$(TARGET_DEBUG) 2
	-./$(TARGET_DEBUG) 3
	-./$(TARGET_DEBUG) 4

run-release: release
	@echo "=== C++ RELEASE (-O3) ==="
	-./$(TARGET_RELEASE) 1
	-./$(TARGET_RELEASE) 2
	-./$(TARGET_RELEASE) 3
	-./$(TARGET_RELEASE) 4