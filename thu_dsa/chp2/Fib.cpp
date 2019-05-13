#include "Fib.h"
#include <iostream>
using std::cout;
using std::endl;

//constructor
Fib::Fib(int n) {
	elem = Vector<int>();
	int prev = 0, current = 1, tmp;

	elem.push_back(prev);
	while (current < n) {
		elem.push_back(current);

		tmp = prev;
		prev = current;
		current = tmp + current;
	}
}

//external interfaces
void Fib::print() {
	for (int ix = 0; ix != elem.getSize(); ++ix)
		cout << elem[ix] << endl;
}

int Fib::get() {
	return elem[elem.getSize() - 1];
}

void Fib::prev() {
	elem.pop_back();
}
