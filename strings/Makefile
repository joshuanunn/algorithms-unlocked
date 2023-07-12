CXXFLAGS = --std=c++17 -O3

all : lcs transform match

lcs : randstring.o lcs.o
	$(CXX) $(CXXFLAGS) -o $@ randstring.o lcs.o

transform : randstring.o transform.o
	$(CXX) $(CXXFLAGS) -o $@ randstring.o transform.o

match : randstring.o match.o
	$(CXX) $(CXXFLAGS) -o $@ randstring.o match.o

randstring.o : include/randstring.cpp include/randstring.hpp
	$(CXX) $(CXXFLAGS) -c $<

lcs.o : lcs.cpp
	$(CXX) $(CXXFLAGS) -c $<

transform.o : transform.cpp
	$(CXX) $(CXXFLAGS) -c $<

match.o : match.cpp
	$(CXX) $(CXXFLAGS) -c $<

clean :
	rm -rf *.o lcs transform match
