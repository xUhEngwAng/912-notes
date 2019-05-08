#include <cassert>
#include <time.h>
#include "Vector.h"

void test_constructor(){
	cout << "--------------run test_constructo--------------" << endl;
	Vector<int> V();
	assert(V.getSize() == 0);
	assert(V.getCapacity() == DEFAULT_CAPACITY);

	

	int A[10] = { 2,3,4,6,1,0,9,8,7,5 };
	Vector<int> V(A, 10);
}