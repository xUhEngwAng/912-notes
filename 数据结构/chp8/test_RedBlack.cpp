#include "RedBlack.h"
#include <iostream>
#include <cassert>
using std::cout;
using std::endl;

void test_constructor();
void test_insert();
void test_remove();

int main(){
	cout << "Running tests." << endl;

	test_constructor();
	test_insert();
	test_remove();

	cout << "All tests passed." << endl;
	system("pause");
	return 0;
}

void test_constructor(){
	RedBlack<int, int> rbTree;
	assert(rbTree.size() == 0);
	auto root = rbTree.root();
	assert(!root);
	assert(ISBLACK(root));
	assert(!ISRED(root));
	assert(BLACK_HEIGHT(root) == 0);
	assert(!BLACK_HEIGHT_CHANGED(root));
}

void test_insert(){
	RedBlack<int, int> rbTree;

	//insert one element
	rbTree.insert(0, 0);
	auto root = rbTree.root();
	assert(rbTree.size() == 1);
	assert(root->data.key == 0);
	assert(BLACK_HEIGHT(root) == 1);
	assert(ISBLACK(root));
	assert(!ISRED(root));
	assert(!root->leftChild && !root->rightChild);

	//insert two
	rbTree.insert(0, 0);
	assert(rbTree.size() == 1);
	assert(!root->leftChild && !root->rightChild);
	rbTree.insert(1, 1);
	assert(rbTree.size() == 2);
	assert(root->data.key == 0);
	assert(BLACK_HEIGHT(root) == 1);
	assert(ISBLACK(root));
	assert(!ISRED(root));
	assert(!root->leftChild && root->rightChild);
	auto right = root->rightChild;
	assert(!ISBLACK(right));
	assert(ISRED(right));
	assert(BLACK_HEIGHT(right) == 1);
	assert(!right->leftChild && !right->rightChild);

	//double red: case one
	rbTree.insert(2, 2);
	root = rbTree.root();
	auto left = root->leftChild;
	right = root->rightChild;
	assert(rbTree.size() == 3);
	assert(root->data.key == 1);
	assert(ISBLACK(root));
	assert(BLACK_HEIGHT(root) == 1);
	assert(left->data.key == 0);
	assert(ISRED(left));
	assert(BLACK_HEIGHT(left) == 1);
	assert(!left->leftChild && !left->rightChild);
	assert(right->data.key == 2);
	assert(ISRED(right));
	assert(BLACK_HEIGHT(right) == 1);
	assert(!right->rightChild && !right->rightChild);

	//double red: case two
	rbTree.insert(3, 3);
	assert(rbTree.size() == 4);
	assert(root->data.key == 1);
	assert(ISBLACK(root));
	assert(BLACK_HEIGHT(root) == 2);
	assert(left->data.key == 0);
	assert(!ISRED(left));
	assert(BLACK_HEIGHT(left) == 1);
	assert(!left->leftChild && !left->rightChild);
	assert(right->data.key == 2);
	assert(!ISRED(right));
	assert(BLACK_HEIGHT(right) == 1);
	assert(right->rightChild && !right->leftChild);
	assert(right->rightChild->data.key == 3);
	assert(ISRED(right->rightChild));

	//continue insertion
	rbTree.insert(4, 4);
	rbTree.insert(5, 5);
	left = root->leftChild;
	right = root->rightChild;
	auto rightleft = right->leftChild;
	auto rightright = right->rightChild;
	assert(rbTree.size() == 6);
	assert(ISBLACK(root));
	assert(ISBLACK(left));
	assert(ISRED(right));
	assert(BLACK_HEIGHT(right) == 2);
	assert(ISBLACK(rightleft) && ISBLACK(rightright));
	assert(!rightright->leftChild && rightright->rightChild);

	//continue
	rbTree.insert(6, 6);
	rbTree.insert(7, 7);
	rbTree.insert(8, 8);
	rbTree.insert(9, 9);
	root = rbTree.root();
	left = root->leftChild;
	right = root->rightChild;
	auto leftleft = left->leftChild;
	auto leftright = left->rightChild;
	rightleft = right->leftChild;
	rightright = right->rightChild;
	auto rightrightleft = rightright->leftChild;
	auto rightrightright = rightright->rightChild;
	assert(rbTree.size() == 10);
	assert(root->data.key == 3);
	assert(BLACK_HEIGHT(root) == 3);
	assert(ISBLACK(root));
	assert(left->data.key == 1);
	assert(BLACK_HEIGHT(left) == 2);
	assert(ISBLACK(left));
	assert(left->parent == root);
	assert(right->data.key == 5);
	assert(ISBLACK(right));
	assert(BLACK_HEIGHT(right) == 2);
	assert(right->parent == root);
	assert(leftleft->data.key == 0);
	assert(ISBLACK(leftleft));
	assert(BLACK_HEIGHT(leftleft) == 1);
	assert(leftleft->parent == left);
	assert(leftright->data.key == 2);
	assert(ISBLACK(leftright));
	assert(BLACK_HEIGHT(leftright) == 1);
	assert(leftright->parent == left);
	assert(rightleft->data.key == 4);
	assert(ISBLACK(rightleft));
	assert(BLACK_HEIGHT(rightleft) == 1);
	assert(rightleft->parent == right);
	assert(rightright->data.key == 7);
	assert(!ISBLACK(rightright) && ISRED(rightright));
	assert(BLACK_HEIGHT(rightright) == 2);
	assert(rightright->parent == right);
	assert(rightrightleft->data.key == 6);
	assert(ISBLACK(rightrightleft));
	assert(BLACK_HEIGHT(rightrightleft) == 1);
	assert(rightrightleft->parent == rightright);
	assert(rightrightright->data.key == 8);
	assert(ISBLACK(rightrightright));
	assert(BLACK_HEIGHT(rightrightright) == 1);
	assert(rightrightright->parent == rightright);
}

void test_remove(){
	RedBlack<int, int> rbTree;
	for (int ix = 0; ix != 10; ++ix)
		rbTree.insert(ix, ix);
	//remove red leaf
	assert(rbTree.remove(8));
	assert(rbTree.size() == 9);
	auto rightright = rbTree.root()->rightChild->rightChild;
	auto rightrightleft = rightright->leftChild;
	auto rightrightright = rightright->rightChild;
	assert(ISRED(rightright));
	assert(BLACK_HEIGHT(rightright) == 2);
	assert(rightrightright->data.key == 9);
	assert(ISBLACK(rightrightright));
	assert(BLACK_HEIGHT(rightrightright));
	assert(rightrightright->parent == rightright);
	
	//double black: case two
	assert(rbTree.remove(6));
	assert(rbTree.size() == 8);
	assert(ISBLACK(rightright));
	assert(rightright->data.key == 7);
	assert(BLACK_HEIGHT(rightright) == 1);
	assert(!rightright->leftChild);
	assert(rightrightright->data.key == 9);
	assert(ISRED(rightrightright));

	//double black: case three
	assert(rbTree.remove(2));
	assert(rbTree.size() == 7);
	auto root = rbTree.root();
	auto left = root->leftChild;
	auto right = root->rightChild;
	auto leftleft = left->leftChild;
	auto rightleft = right->leftChild;
	rightright = right->rightChild;
	assert(root->data.key == 3);
	assert(BLACK_HEIGHT(root) == 2);
	assert(left->data.key == 1);
	assert(ISBLACK(left));
	assert(BLACK_HEIGHT(left) == 1);
	assert(left->parent == root);
	assert(!left->rightChild);
	assert(leftleft->data.key == 0);
	assert(ISRED(leftleft));
	assert(leftleft->parent == left);
	assert(BLACK_HEIGHT(leftleft) == 1);
	assert(right->data.key == 5);
	assert(ISRED(right));
	assert(BLACK_HEIGHT(right) == 2);
	assert(right->parent == root);
	assert(rightleft->data.key == 4);
	assert(rightright->data.key == 7);
	assert(rightleft->parent == right && rightleft->parent == right);
	assert(ISBLACK(rightleft) && ISBLACK(rightright));
	assert(rightright->rightChild->data.key == 9 && ISRED(rightright->rightChild));

	//double black: case four
	assert(rbTree.remove(1));
	assert(rbTree.remove(0));
	assert(rbTree.size() == 5);
	root = rbTree.root();
	left = root->leftChild;
	auto leftright = left->rightChild;
	right = root->rightChild;
	rightright = right->rightChild;
	assert(root->data.key == 5);
	assert(ISBLACK(root));
	assert(BLACK_HEIGHT(root) == 2);
	assert(left->data.key == 3);
	assert(ISBLACK(left));
	assert(BLACK_HEIGHT(left));
	assert(left->parent == root && !left->leftChild);
	assert(leftright->data.key == 4);
	assert(ISRED(leftright));
	assert(leftright->parent == left);
	assert(right->data.key == 7);
	assert(ISBLACK(right));
	assert(right->parent == root);
	assert(rightright->data.key == 9);
	assert(ISRED(rightright));
	assert(rightright->parent == right);

	//double black: case one
	assert(rbTree.remove(3));
	assert(rbTree.remove(4));
	assert(rbTree.size() == 3);
	root = rbTree.root();
	right = root->rightChild;
	left = root->leftChild;
	assert(root->data.key == 7);
	assert(ISBLACK(root));
	assert(BLACK_HEIGHT(root) == 2);
	assert(left->data.key == 5);
	assert(!ISRED(left));
	assert(left->parent == root);
	assert(!left->leftChild && !left->rightChild);
	assert(right->data.key == 9);
	assert(!ISRED(right));
	assert(right->parent == root);
	assert(!right->leftChild && !right->rightChild);
}
