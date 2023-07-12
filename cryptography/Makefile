CXXFLAGS = --std=c++17 -O3

all : euclid modexp

euclid : euclid.o
	$(CXX) $(CXXFLAGS) -o $@ euclid.o

modexp : modexp.o
	$(CXX) $(CXXFLAGS) -o $@ modexp.o

euclid.o : euclid.cpp
	$(CXX) $(CXXFLAGS) -c $<

modexp.o : modexp.cpp
	$(CXX) $(CXXFLAGS) -c $<

clean :
	rm -rf *.o euclid modexp
