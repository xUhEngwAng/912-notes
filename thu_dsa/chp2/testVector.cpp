#include "Vector.h"
#include <cassert>
#include <iostream>

using std::cout;
using std::endl;

void test_constructor();
void test_access();
void test_insert();
void test_delete();
void test_find();
void test_unique();
void test_search();
void test_map();

int main(){
	test_constructor();
	test_access();
	test_insert();
	test_delete();
	test_find();
	test_unique();
	test_search();
	test_map();

	system("pause");
	return 0;
}

void test_constructor() {
	cout << "run test_constructor...";
	Vector<int> V;
	assert(V.empty());
	assert(V.getCapacity() == DEFAULT_CAPACITY);

	Vector<int> V1(10);
	assert(V1.empty());
	assert(V1.getCapacity() == 10);

	int A[10] = { 2,3,4,6,1,0,9,8,7,5 };
	Vector<int> V2(A, 10);
	assert(V2.getSize() == 10);
	assert(V2.getCapacity() == 20);
	assert(!V2.empty());

	Vector<int> V3(A, 3, 7);
	assert(V3.getSize() == 4);
	assert(V3.getCapacity() == 8);

	Vector<int> V4(V3);
	assert(V4.getSize() == 4);
	assert(V4.getCapacity() == 8);

	Vector<int> V5(V4, 2, 4);
	assert(V5.getSize() == 2);
	assert(V5.getCapacity() == 4);

	cout << "test passed." << endl;
}

void test_access(){
	cout << "run test_access...";
	int A[] = { 2,3,4,6,1,0,9,8,7,5 };
	Vector<int> v(A, 10);

	//test Vector<T>::get method
	for (int ix = 0; ix != 10; ++ix)
		assert(v.get(ix) == A[ix]);

	//test Vector<T>::replace method
	int val = 12;
	v.replace(0, 12);
	v.replace(5, 12);
	assert(v[0] == 12);
	assert(v[5] == 12);

	//test [] 
	v[1] = val;
	v[2] = val;
	assert(v[1] == val);
	assert(v[2] == val);

	cout << "test passed." << endl;
}

void test_insert(){
	cout << "run test_insert...";
	int A[] = { 2,3,4,6,1,0,9,8,7,5 };
	Vector<int> v(A, 10);
	v.push_back(12);
	v.insert(2, 15);
	assert(v.getSize() == 12);
	assert(v.getCapacity() == 20);
	assert(v[11] == 12);
	assert(v[2] == 15);

	cout << "test passed." << endl;
}

void test_delete(){
	cout << "run test_delete...";
	int A[] = { 2,3,4,6,1,0,9,8,7,5 };
	Vector<int> v(A, 10);

	assert(v.pop_back() == 5);
	assert(v.pop(5) == 0);
	assert(v.pop(4, 6) == 9);
	assert(v.getSize() == 6);
	assert(v.remove(8) == 4);
	assert(v.remove(12) == -1);

	cout << "test passed." << endl;
}

void test_find(){
	cout << "run test_find...";
	int A[] = { 2,3,4,6,1,0,9,8,7,5 };
	Vector<int> v(A, 10);

	assert(v.find(4) == 2);
	assert(v.find(5) == 9);
	assert(v.find(11) == -1);
	assert(v.find(4, 2, 8) == 2);
	assert(v.find(4, 5, 8) == 4);

	cout << "test passed." << endl;
}

void test_unique(){
	cout << "run test_unique...";
	int A[] = { 2,3,4,6,1,0,9,8,7,5,9,2,3,5,6,3,3,5 };
	Vector<int> v(A, 0, 10);
	assert(v.unique() == 0);
	assert(v.getSize() == 10);
	
	v = Vector<int>(A, 18);
	assert(v.unique() == 8);
	assert(v.getSize() == 10);
	assert(v[9] == 5);

	// test basic function
	int B[] = { 1,1,1,2,2,3,3,3,3,3,4,4,5,6,6,7,7,7,8,8,9,9,9,9,9,9 };
	v = Vector<int>(B, 26);
	assert(v.uniquify() == 17);
	assert(v.getSize() == 9);
	for (int ix = 0; ix != 9; ++ix)
		assert(v[ix] == ix + 1);

	//test running time
	//remain to be done

	cout << "test passed." << endl;
}

void test_search(){
	cout << "run test_search...";
	int A[] = { 2,5,6,8,9,10,13,14,17,19,21,22,30,32 };
	Vector<int> v(A, 14);

	//test search
	assert(v.search(14, 0, v.getSize()) == 7);
	assert(v.search(14, 9, v.getSize()) == 8);
	assert(v.search(1, 0, v.getSize()) == -1);

	//test binary_search
	assert(v.binary_search(14, 0, v.getSize()) == 7);
	assert(v.binary_search(15, 0, v.getSize()) == 7);
	assert(v.binary_search(14, 9, v.getSize()) == 8);
	
	int B[] = { 1,1,1,2,2,3,3,3,3,3,4,4,5,6,6,7,7,7,8,8,9,9,9,9,9,9 };
	v = Vector<int>(B, 0, 26);
	assert(v.binary_search(3, 0, v.getSize()) == 9);
	assert(v.binary_search(9, 0, v.getSize()) == 25);
	assert(v.binary_search(0, 0, v.getSize()) == -1);

	cout << "test passed." << endl;
}

template <typename T>
void Increase(T & elem) { ++elem; }
void test_map(){
	cout << "run test_map...";
	float A[] = { 2,3,4,6,1,0,9,8,7,5,9,2,3,5,6,3,3,5 };
	Vector<float> v(A, 10);

	v.map(Increase);
	for (int ix = 0; ix != v.getSize(); ++ix)
		assert(v[ix] == A[ix] + 1);

	cout << "test passed." << endl;
}
