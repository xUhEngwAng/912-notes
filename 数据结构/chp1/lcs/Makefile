lcstest: lcs.o test_lcs.o
	g++ lcs.o test_lcs.o -o lcstest

lcs.o: lcs.cpp lcs.h
	g++ -std=c++11 -c lcs.cpp 

test_lcs.o: test_lcs.cpp lcs.cpp
	g++ -c test_lcs.cpp

clean:
	rm -rf *.o lcstest
	@echo "Clean Done!"
