#include "Fib.h"
#include "Vector.h"
#include <time.h>
#include <cassert>
#include <iostream>

#define NUMOFLOOPS 1000
#define ELEMENT_NUM 10000

using std::cout;
using std::endl;

void test_constructor();
void test_access();
void test_insert();
void test_delete();
void test_find();
void test_unique();
void test_search();
void test_search_performance();
void test_map();
void test_bubbleSort();
void test_mergeSort();

int main(){
	test_constructor();
	test_access();
	test_insert();
	test_delete();
	test_find();
	test_unique();
	test_search();
	test_search_performance();
	test_bubbleSort();
	test_mergeSort();
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

	//test fib_search
	assert(v.fib_search(14, 0, v.getSize()) == 7);
	assert(v.fib_search(14, 9, v.getSize()) == 8);
	assert(v.fib_search(1, 0, v.getSize()) == -1);

	//test interpolation_search
	assert(v.interpolation_search(14, 0, v.getSize()) == 7);
	assert(v.interpolation_search(14, 9, v.getSize()) == 8);
	assert(v.interpolation_search(1, 0, v.getSize()) == -1);

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

void test_search_performance(){
	cout << "run test_search_performance..." << endl;
	srand((unsigned)time(NULL));
	Vector<int> v1;
	for (int ix = 0; ix != ELEMENT_NUM; ++ix)
		v1.push_back(rand());
	v1.mergeSort();

	//test binary search
	cout << "run binary_search...";
	clock_t begin = clock();
	for(int ix = 0; ix != NUMOFLOOPS; ++ix)
		assert(v1.binary_search(v1[824], 0, v1.getSize()) == 824);
	clock_t end = clock();
	cout << "Running time: " << end - begin << endl;

	//test plain old search
	cout << "run plain old search...";
	begin = clock();
	for (int ix = 0; ix != NUMOFLOOPS; ++ix)
		assert(v1.search(v1[824], 0, v1.getSize()) == 824);
	end = clock();
	cout << "Running time: " << end - begin << endl;

	//test fib search
	cout << "run fib_search...";
	begin = clock();
	for (int ix = 0; ix != NUMOFLOOPS; ++ix)
		assert(v1.fib_search(v1[824], 0, v1.getSize()) == 824);
	end = clock();
	cout << "Running time: " << end - begin << endl;

	//test interpolation search
	cout << "run binary_search...";
	begin = clock();
	for (int ix = 0; ix != NUMOFLOOPS; ++ix)
		assert(v1.interpolation_search(v1[824], 0, v1.getSize()) == 824);
	end = clock();
	cout << "Running time: " << end - begin << endl;

	cout << "test passed." << endl;
}

void test_bubbleSort(){
	cout << "run test_bubbleSort...\n";
	int A[] = { 2,3,4,6,1,0,9,8,7,5 };
	Vector<int> v(A, 10);
	assert(v.unordered() == 5);

	v.bubbleSort();
	assert(v.unordered() == 0);
	for (int ix = 0; ix != 10; ++ix)
		assert(v[ix] == ix);
	
	srand((unsigned)time(NULL));
	Vector<int> v1;
	for (int ix = 0; ix != ELEMENT_NUM; ++ix)
		v1.push_back(rand());
	assert(v1.getSize() == ELEMENT_NUM);
	assert(v1.unordered() != 0);

	clock_t begin = clock();
	v1.bubbleSort();
	clock_t end = clock();
	assert(v.unordered() == 0);
	cout << "Running time: " << end - begin << endl;

	cout << "test passed." << endl;
}

void test_mergeSort(){
	cout << "run test_mergeSort..." << endl;
	int A[] = { 2,3,4,6,1,0,9,8,7,5 };
	Vector<int> v(A, 10);
	assert(v.unordered() == 5);

	v.mergeSort();
	assert(v.unordered() == 0);
	for (int ix = 0; ix != 10; ++ix)
		assert(v[ix] == ix);

	srand((unsigned)time(NULL));
	Vector<int> v1;
	for (int ix = 0; ix != ELEMENT_NUM; ++ix)
		v1.push_back(rand());
	assert(v1.getSize() == ELEMENT_NUM);
	assert(v1.unordered() != 0);

	clock_t begin = clock();
	v1.mergeSort();
	clock_t end = clock();
	assert(v.unordered() == 0);
	cout << "Running time: " << end - begin << endl;

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
