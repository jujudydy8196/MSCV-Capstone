CXX = g++ -std=c++11
CXXFLAGS = -Wall -g

all: cosaliency
cosaliency: cosaliency.o
	$(CXX) $(CXXFLAGS)  cosaliency.o -o cosal `pkg-config --cflags --libs opencv`
cosaliency.o: cosaliency.cpp config.h
	$(CXX) $(CXXFLAGS) -c cosaliency.cpp

