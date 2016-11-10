
CXX = g++ -std=c++11
CXXFLAGS = -Wall -g

all: cointerest
cointerest: cointerest.o
	$(CXX) $(CXXFLAGS)  cointerest.o -o coint `pkg-config --cflags --libs opencv`
cointerest.o: cointerest.cpp base.h ../cosaliency/config.h
	$(CXX) $(CXXFLAGS) -c cointerest.cpp

