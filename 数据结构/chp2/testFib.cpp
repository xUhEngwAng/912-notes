#include "Vector.h"
#include <cassert>
#include <iostream>

using std::cout;
using std::endl;

void test_fib_constructor();
void test_get_and_prev();

int test_fib_main(){
	test_fib_constructor();
	test_get_and_prev();

	system("pause");
	return 0;
}

void test_fib_constructor(){
	cout << "run test_constructor...";
	Fib fib(100);

	cout << "test passed." << endl;
}

void test_get_and_prev(){
	cout << "run test_get_and_prev...";
	Fib fib(100);
	assert(fib.get() == 89);
	fib.prev();
	assert(fib.get() == 55);
	fib.prev();
	assert(fib.get() == 34);

	cout << "test passed." << endl;
}
