CXX = g++
CXXFLAGS = -O3 -Wall -Wextra -Werror -std=c++20

SRC = main.cpp
TARGET = bench_cpp

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $@

run: $(TARGET)
	taskset -c 0 ./$(TARGET)

clean:
	rm -f $(TARGET)