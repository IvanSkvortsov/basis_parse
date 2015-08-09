CXX = g++
#CXXFLAGS = -O2 -Wall -pedantic -Wextra -Weffc++ -lboost_regex
CXXFLAGS = -lboost_regex
#CXXFLAGS = -O2 -Wall -pedantic -Wextra -Weffc++

#CXX = icpc
#CXXFLAGS = -O3 -lboost_regex

.PHONY: testall
testall: test01 test02 test03

test01: parser.exe HCuN.basis
	./parser.exe HCuN.basis > $@.log 2> $@.err
	cat $@.log
	cat $@.err

test02: parser.exe HCuNbad.basis
	./parser.exe HCuNbad.basis > $@.log 2> $@.err
	cat $@.log
	cat $@.err

test03: parser.exe HCuNbad2.basis
	./parser.exe HCuNbad2.basis > $@.log 2> $@.err

parser.exe: main.cpp SingleBasisSet.hpp GamessFormat.hpp Format.hpp
	$(CXX) main.cpp -o $@ $(CXXFLAGS) 

.PHONY: clean
clean:
	rm -rf test*.log parser.exe test*.err



