#include "binTree.h"
#include <iostream>
#include <cassert>

using std::cout;
using std::endl;

void test_insert();
void test_size();
void test_preTraversal();
void test_inTraversal();
void test_postTraversal();
void test_levelTraversal();

int main(){
	cout << "Running test." << endl;

	test_insert();
	test_size();
	test_preTraversal();
	test_inTraversal();
	test_postTraversal();
	test_levelTraversal();

	cout << "All tests passed." << endl;
	system("pause");
	return 0;
}

void test_insert(){
	BinTree<int> itree;
	assert(itree.size() == 0);
	assert(itree.empty());
	assert(itree.root() == nullptr);

	itree.insertAsRoot(8);
	BinNodePosi(int) root = itree.root();
	assert(itree.size() == 1);
	assert(!itree.empty());
	assert(root->data == 8);
	assert(root->height == 0);
	assert(root->leftChild == nullptr);
	assert(root->rightChild == nullptr);

	itree.insertAsLC(root, 7);
	itree.insertAsRC(root, 14);
	BinNodePosi(int) left  = root->leftChild;
	BinNodePosi(int) right = root->rightChild;
	assert(itree.size() == 3);
	assert(root->height == 1);
	assert(left->data == 7);
	assert(right->data == 14);
	assert(left->parent == root);
	assert(right->parent == root);
	assert(left->leftChild == nullptr);
	assert(right->leftChild == nullptr);
	assert(right->rightChild == nullptr);
	assert(left->height == 0 && right->height == 0);
}

void test_size(){
	BinTree<int> intTree;
	BinNodePosi(int) root = intTree.insertAsRoot(8);
	assert(root->size() == 1);
	intTree.insertAsLC(root, 7);
	assert(root->size() == 2);
	intTree.insertAsRC(root, 14);
	assert(root->size() == 3);
}

template <typename T>
struct Print{
	virtual void operator()(T const &val){
		cout << val << " ";
	}
};

void test_preTraversal(){
	BinTree<int> intTree;
	cout << "tree pre-order traversal: " << endl;

	//construct a binary tree
	BinNodePosi(int) root  = intTree.insertAsRoot(8);
	BinNodePosi(int) left  = intTree.insertAsLC(root, 7);
	BinNodePosi(int) right = intTree.insertAsRC(root, 14);
	intTree.insertAsLC(left, 4);
	intTree.insertAsRC(left, 3);
	intTree.insertAsLC(right, 12);
	intTree.insertAsRC(right, 18);

	root->preOrder_Re(Print<int>());
	cout << endl;
	root->preOrder_It1(Print<int>());
	cout << endl;
	root->preOrder_It2(Print<int>());
	cout << endl;
}

void test_inTraversal(){
	BinTree<int> intTree;
	cout << "tree in-order traversal: " << endl;

	//construct a binary tree
	BinNodePosi(int) root  = intTree.insertAsRoot(8);
	BinNodePosi(int) left  = intTree.insertAsLC(root, 7);
	BinNodePosi(int) right = intTree.insertAsRC(root, 14);
	intTree.insertAsLC(left, 4);
	intTree.insertAsRC(left, 3);
	intTree.insertAsLC(right, 12);
	intTree.insertAsRC(right, 18);

	root->inOrder_Re(Print<int>());
	cout << endl;
	root->inOrder_It(Print<int>());
	cout << endl;
}

void test_postTraversal(){
	BinTree<int> intTree;
	cout << "tree post-order traversal: " << endl;

	//construct a binary tree
	BinNodePosi(int) root = intTree.insertAsRoot(8);
	BinNodePosi(int) left = intTree.insertAsLC(root, 7);
	BinNodePosi(int) right = intTree.insertAsRC(root, 14);
	intTree.insertAsLC(left, 4);
	intTree.insertAsRC(left, 3);
	intTree.insertAsLC(right, 12);
	intTree.insertAsRC(right, 18);

	root->postOrder_Re(Print<int>());
	cout << endl;
}

void test_levelTraversal(){
	BinTree<int> intTree;
	cout << "tree level-order traversal: " << endl;

	//construct a binary tree
	BinNodePosi(int) root = intTree.insertAsRoot(8);
	BinNodePosi(int) left = intTree.insertAsLC(root, 7);
	BinNodePosi(int) right = intTree.insertAsRC(root, 14);
	intTree.insertAsLC(left, 4);
	intTree.insertAsRC(left, 3);
	intTree.insertAsLC(right, 12);
	intTree.insertAsRC(right, 18);

	root->levelOrder(Print<int>());
	cout << endl;
}
