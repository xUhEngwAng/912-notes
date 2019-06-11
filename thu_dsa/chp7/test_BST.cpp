#include "BST.h"
#include <iostream>
#include <cassert>
#include <string>

using std::cout;
using std::endl;
using std::string;

void test_insert_and_search();
void test_remove();
void test_index();

int main(){
	cout << "Running test." << endl;

	test_insert_and_search();
	test_remove();
	test_index();

	cout << "All tests passed." << endl;
	system("pause");
	return 0;
}

void test_insert_and_search(){
	BST<int, string> bstTree;
	int keys[] = { 5, 8, 2, 7, 6, 4, 9, 1, 3 };
	string values[] = { "five", "eight", "two", "seven", "six", "four", "nine", "one", "three" };

	for (int ix = 0; ix != 9; ++ix)
		bstTree.insert(keys[ix], values[ix]);
	assert(bstTree.size() == 9);

	//test duplicates
	for (int ix = 0; ix != 9; ++ix)
		bstTree.insert(keys[ix], values[8 - ix]);
	assert(bstTree.size() == 9);

	//test search
	for(int ix = 0; ix != 9; ++ix)
		assert(bstTree.search(keys[ix])->data.value == values[ix]);

	//test BST characteristics 
#define T entry<int, string>
	BinNodePosi(T) x = bstTree.search(1);
	for (int ix = 1; x; ++ix, x = x->succ())
		assert(x->data.key == ix);
#undef T
}

void test_remove(){
	BST<int, string> bstTree;
	int keys[] = { 5, 8, 2, 7, 6, 4, 9, 1, 3 };
	string values[] = { "five", "eight", "two", "seven", "six", "four", "nine", "one", "three" };
	for (int ix = 0; ix != 9; ++ix)
		bstTree.insert(keys[ix], values[ix]);

	assert(bstTree.root()->data.key == 5);
	//remove leaf
	assert(bstTree.remove(6));//no succ
	assert(bstTree.size() == 8);
	assert(bstTree.search(6) == nullptr);

	//remove intermediate node
	assert(bstTree.remove(4));
	assert(bstTree.size() == 7);
	assert(bstTree.search(4) == nullptr);
	assert(bstTree.search(3)->parent->data.key == 2);
	assert(bstTree.search(2)->rightChild->data.key == 3);

	//remove root node
	assert(bstTree.remove(5));
	assert(bstTree.size() == 6);
	assert(bstTree.root()->data.key == 7);
	assert(bstTree.root()->leftChild->data.key == 2 && bstTree.root()->rightChild->data.key == 8);

	//test BST topology
#define T entry<int, string>
	BinNodePosi(T) x = bstTree.search(1);
	int remainedKeys[] = { 1, 2, 3, 7, 8, 9 };
	for (int ix = 0; x; ++ix, x = x->succ()) {
		assert(x->data.key == remainedKeys[ix]);
	}
#undef T
}

void test_index(){
	BST<int, string> bstTree;
	int keys[] = { 5, 8, 2, 7, 6, 4, 9, 1, 3 };
	string values[] = { "five", "eight", "two", "seven", "six", "four", "nine", "one", "three" };
	for (int ix = 0; ix != 9; ++ix)
		bstTree.insert(keys[ix], values[ix]);

	for (int ix = 0; ix != 9; ++ix)
		assert(bstTree[keys[ix]] == values[ix]);
}
