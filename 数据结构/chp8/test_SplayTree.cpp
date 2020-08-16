#include "SplayTree.h"
#include <time.h>
#include <iostream>
#include <cassert>

using std::cout;
using std::endl;

void test_insert_and_search();
void test_remove();

int main(){
	cout << "Running tests." << endl;

	test_insert_and_search();
	test_remove();

	cout << "All tests passed." << endl;
	system("pause");
	return 0;
}

void test_insert_and_search(){
	SplayTree<int, int> sTree;

	//orderly insert
	for (int ix = 0; ix != 32; ++ix)
		sTree.insert(ix, ix);
	assert(sTree.size() == 32);
	assert(sTree.root()->data.key == 31);
	assert(sTree.root()->height == 31);

	//test search
	assert(sTree.search(0)->data.key == 0);
	assert(sTree.root()->data.key == 0);
	assert(sTree.root()->height == 17);
	assert(sTree.root()->rightChild->data.key = 31);
	assert(sTree.root()->leftChild == nullptr);
	assert(sTree.root()->rightChild->height == 16);

	assert(sTree.search(1)->data.key == 1);
	assert(sTree.root()->data.key == 1);
	assert(sTree.root()->height == 10);
	assert(sTree.root()->rightChild->data.key = 31);
	assert(sTree.root()->leftChild->data.key  == 0);
	assert(sTree.root()->rightChild->height == 9);

	assert(sTree.search(2)->data.key == 2);
	assert(sTree.root()->data.key == 2);
	assert(sTree.root()->height == 7);
	assert(sTree.root()->rightChild->data.key = 27);
	assert(sTree.root()->leftChild->data.key == 1);
	assert(sTree.root()->rightChild->height == 6);
}

void test_remove(){
	SplayTree<int, int> sTree;
	//orderly insert
	for (int ix = 0; ix != 32; ++ix)
		sTree.insert(ix, ix);
	assert(sTree.size() == 32);
	assert(sTree.root()->data.key == 31);
	assert(sTree.root()->height == 31);

	assert(!sTree.remove(33));
	assert(!sTree.remove(35));
	assert(sTree.size() == 32);

	assert(sTree.remove(31));
	assert(sTree.size() == 31);
	assert(sTree.root()->data.key == 30);
	assert(sTree.root()->height == 30);

	assert(sTree.remove(0));
	assert(sTree.size() == 30);
	assert(sTree.root()->data.key == 1);
	assert(sTree.root()->height == 9);
	assert(sTree.root()->rightChild->data.key == 27);
	assert(sTree.root()->rightChild->height == 8);
	assert(sTree.root()->leftChild == nullptr);

	assert(sTree.remove(27));
	assert(sTree.size() == 29);
	assert(sTree.root()->data.key == 28);
	assert(sTree.root()->leftChild->data.key == 1);
	assert(sTree.root()->rightChild->data.key == 29);
	assert(sTree.root()->height == 9);
	assert(sTree.root()->leftChild->height == 8);
}
