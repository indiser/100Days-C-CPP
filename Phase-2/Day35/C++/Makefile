CXX = g++
CXXFLAGS = -Wall -Wextra -O2 -std=c++20
TARGET = reactor_server
SRC = async.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $@ $^

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: all run clean