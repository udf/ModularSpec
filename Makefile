CXX=g++
CXXFLAGS=-g -std=c++1z -Wall -pedantic -lopenal -lfftw3
SRCS=$(wildcard *.cpp)

all:
	$(CXX) $(CXXFLAGS) -lncurses $(SRCS) -o example

clean:
	rm $(BINS)