#ifndef FIB_H_
#define FIB_H_

#include "Vector.h"

class Fib{
protected:
	Vector<int> elem;
public:
	//constructor
	Fib(int n);

	void print();
	int get();
	void prev();
};

#endif
