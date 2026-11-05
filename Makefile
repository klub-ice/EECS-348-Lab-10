CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra

matrix: main.cpp
	$(CXX) $(CXXFLAGS) -o matrix main.cpp

clean:
	rm -f matrix