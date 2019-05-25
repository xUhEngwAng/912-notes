#include "Vector.h"

//constructor
Fib::Fib(int n) {
	Vector<int>();
	int prev = 0, current = 1, tmp;

	push_back(prev);
	while (current < n) {
		push_back(current);

		tmp = prev;
		prev = current;
		current = tmp + current;
	}
}

//external interfaces
void Fib::print() {
	for (int ix = 0; ix != getSize(); ++ix)
		cout << operator[](ix) << endl;
}

int Fib::get() {
	return operator[](getSize() - 1);
}

void Fib::prev() {
	pop_back();
}
