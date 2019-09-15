#include "CompleteBinaryHeap.h"
#include "LeftistHeap.h"
#include <iostream>
#include <cassert>
using std::cout;
using std::endl;

void test_insert();
void test_max();
void test_heapify();

int main(){
	cout << "Running tests" << endl;

	test_heapify();
	test_insert();
	test_max();

	cout << "All tests passed." << endl;
	system("pause");
	return 0;
}

void test_heapify(){
	int keys[] = { 5, 4, 7, 9, 1, 2, 8, 3, 6, 0 };
	int results[] = { 9, 6, 8, 5, 1, 2, 7, 3, 4, 0};
	entry<int, int> src[10];
	for (int ix = 0; ix != 10; ++ix)
		src[ix] = entry<int, int>(keys[ix], 0);
	CBHeap<int, int> heap(src, 10);
	assert(heap.size() == 10);
	for (int ix = 0; ix != 10; ++ix) {
		assert(heap[ix].key == results[ix]);
	}
}

void test_insert(){
	int keys[] = { 5, 4, 7, 9, 1, 2, 8, 3, 6, 0 };
	int results[] = { 9, 7, 8, 6, 1, 2, 5, 3, 4, 0 };
	CBHeap<int, int> heap;
	for(int ix = 0; ix != 10; ++ix){
		assert(heap.size() == ix);
		heap.insert(entry<int, int>(keys[ix], 0));
	}
	for (int ix = 0; ix != 10; ++ix)
		assert(heap[ix].key == results[ix]);
}

void test_max(){
	int keys[] = { 5, 4, 7, 9, 1, 2, 8, 3, 6, 0 };
	entry<int, int> src[10];
	for (int ix = 0; ix != 10; ++ix)
		src[ix] = entry<int, int>(keys[ix], 0);
	CBHeap<int, int> heap(src, 10);
	for (int ix = 0; ix != 10; ++ix) {
		assert(heap.size() == 10 - ix);
		assert(heap.getMax().key == 9 - ix);
		assert(heap.delMax().key == 9 - ix);
		/*
		for (int jx = 0; jx != 9 - ix; ++jx)
			cout << heap[jx].key << " ";
		cout << endl;
		*/
	}
}
