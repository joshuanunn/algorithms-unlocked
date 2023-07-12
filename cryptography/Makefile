CXXFLAGS = --std=c++17 -O3

all : euclid

euclid : euclid.o
	$(CXX) $(CXXFLAGS) -o $@ euclid.o

euclid.o : euclid.cpp
	$(CXX) $(CXXFLAGS) -c $<

clean :
	rm -rf *.o euclid
