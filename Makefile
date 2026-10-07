# COSC1114 Project 2 - Memory Allocation
#
# make or make all : builds firstfit, bestfit and quickfit
# make clean       : gets rid of object and executable files

CXX = g++
CXXFLAGS = -Wall -Werror -g

all: firstfit bestfit quickfit

firstfit: main.o mem_common.o firstfit.o
	$(CXX) $(CXXFLAGS) -o firstfit main.o mem_common.o firstfit.o

bestfit: main.o mem_common.o bestfit.o
	$(CXX) $(CXXFLAGS) -o bestfit main.o mem_common.o bestfit.o

quickfit: main.o mem_common.o quickfit.o
	$(CXX) $(CXXFLAGS) -o quickfit main.o mem_common.o quickfit.o

main.o: main.cpp allocator.h mem_common.h
	$(CXX) $(CXXFLAGS) -c main.cpp

mem_common.o: mem_common.cpp mem_common.h allocator.h
	$(CXX) $(CXXFLAGS) -c mem_common.cpp

firstfit.o: firstfit.cpp allocator.h mem_common.h
	$(CXX) $(CXXFLAGS) -c firstfit.cpp

bestfit.o: bestfit.cpp allocator.h mem_common.h
	$(CXX) $(CXXFLAGS) -c bestfit.cpp

quickfit.o: quickfit.cpp allocator.h mem_common.h
	$(CXX) $(CXXFLAGS) -c quickfit.cpp

clean:
	rm -f *.o firstfit bestfit quickfit

.PHONY: all clean
