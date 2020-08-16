OBJS = max2.o max2test.o
CC = g++
CFLAG = -c

max2test: $(OBJS)
	$(CC) $(OBJS) -o max2test

max2.o: max2.cpp max2.h
	$(CC) $(CFLAG) max2.cpp

max2test.o: max2test.cpp
	$(CC) -std=c++11 $(CFLAG) max2test.cpp

clean:
	rm -rf *.o max2test
	@echo "Clean done!"
