#include "AVL.h"
#include <iostream>
#include <cassert>
#include <string>

using std::cout;
using std::endl;
using std::string;

void test_insert();
void test_remove();

int main(){
	cout << "Running tests." << endl;

	test_insert();
	test_remove();

	cout << "All tests passed." << endl;
	system("pause");
	return 0;
}

void test_insert(){
	AVL<int, string> avlTree;
	string values[] = { "zero", "one", "two", "three", "four", "five", "six", "seven", "eight", "nine" };
	for (int ix = 0; ix != 10; ++ix)
		avlTree.insert(ix, values[ix]);
	auto root = avlTree.root();
	assert(avlTree.size() == 10);
	assert(root->data.key == 3);
	assert(root->height == 3);
	assert(root->leftChild->data.key == 1);
	assert(root->rightChild->data.key == 7);

	//test BST characteristics
	auto x = avlTree.search(0);
	for (int ix = 0; ix != 10; ++ix, x = x->succ())
		assert(x->data.key == ix);
	assert(x == nullptr);

}

void test_remove(){
	AVL<int, string> avlTree;
	string values[] = { "zero", "one", "two", "three", "four", "five", "six", "seven", "eight", "nine" };
	for (int ix = 0; ix != 10; ++ix)
		avlTree.insert(ix, values[ix]);

	assert(avlTree.root()->height == 3);

	avlTree.remove(1);
	assert(avlTree.size() == 9);
	assert(avlTree.root()->height == 3);
	assert(avlTree.root()->data.key == 3);
	assert(avlTree.root()->leftChild->data.key == 2);
	assert(avlTree.root()->rightChild->data.key == 7);
	avlTree.remove(0);
	assert(avlTree.size() == 8);
	assert(avlTree.root()->height == 3);
	assert(avlTree.root()->data.key == 5);
	assert(avlTree.root()->leftChild->data.key == 3);
	assert(avlTree.root()->leftChild->height == 1);
	assert(avlTree.root()->rightChild->data.key == 7);
	assert(avlTree.root()->rightChild->height == 2);

	avlTree.remove(5);
	assert(avlTree.size() == 7);
	assert(avlTree.root()->data.key == 6);
	assert(avlTree.root()->height == 2);
	assert(avlTree.root()->leftChild->data.key == 3);
	assert(avlTree.root()->leftChild->height == 1);
	assert(avlTree.root()->rightChild->data.key == 8);
	assert(avlTree.root()->rightChild->height == 1);
}
