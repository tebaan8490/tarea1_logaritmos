CXX      := g++
CXXFLAGS := -std=c++17 -O2 -Wall -Wextra
INCLUDES := -Iinclude

SRC_COMMON := src/io.cpp src/rtree.cpp src/utils.cpp

.PHONY: all experiment bonus clean

all: experiment

experiment: $(SRC_COMMON) src/main.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) $^ -o experiment

clean:
	rm -f experiment bonus data/*.bin