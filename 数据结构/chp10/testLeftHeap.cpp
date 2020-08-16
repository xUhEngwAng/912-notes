#include "LeftistHeap.h"
#include "../chp4/Queue.h"
#include <iostream>
#include <cassert>
using std::cout;
using std::endl;

void test_insert();
void test_max();

int main(){
	cout << "Running tests" << endl;

	test_insert();
	test_max();

	cout << "All tests passed." << endl;
	system("pause");
	return 0;
}

#define T  entry<int, int>
void test_insert(){
	int keys[] = { 5, 4, 7, 9, 1, 2, 8, 3, 6, 0 };
	int levelOrder[] = { 9, 8, 7, 2, 3, 5, 6, 1, 4, 0 };
	LeftHeap<int, int> heap;
	for (int ix = 0; ix != 10; ++ix) {
		assert(heap.size() == ix);
		heap.insert(entry<int, int>(keys[ix], 0));
	}
	//level-order traversal
	int ix = 0;
	Queue<BinNodePosi(T)> q;
	BinNodePosi(T) curr;
	q.enqueue(heap.root());
	while(!q.empty()){
		curr = q.dequeue();
		assert(curr->data.key == levelOrder[ix++]);
		if (curr->leftChild) q.enqueue(curr->leftChild);
		if (curr->rightChild) q.enqueue(curr->rightChild);
	}

	cout << "test_insert passed." << endl;
}

void test_max(){
	int keys[] = { 5, 4, 7, 9, 1, 2, 8, 3, 6, 0 };
	LeftHeap<int, int> heap;
	for (int ix = 0; ix != 10; ++ix) {
		assert(heap.size() == ix);
		heap.insert(entry<int, int>(keys[ix], 0));
	}
	for(int ix = 0; ix != 10; ++ix){
		assert(heap.getMax().key == 9 - ix);
		assert(heap.delMax().key == 9 - ix);
		assert(heap.size() == 9 - ix);
	}
	cout << "test_max passed." << endl;
}
