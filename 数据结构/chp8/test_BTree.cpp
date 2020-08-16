#include "BTree.h"
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
	BTree<int> tree(4);
	int insert_sequence[] = { 25, 36, 17, 48, 32, 5, 67, 72, 99, 19, 22 };
	int ix = 0;

	//insert __root
	assert(tree.getSize() == 0);
	assert(tree.insert(insert_sequence[ix++]));
	assert(tree.getSize() == 1);
	assert(tree.root()->keys[0] == 25);
	assert(tree.root()->keys.getSize() == 1);
	assert(tree.root()->parent == nullptr);

	//insert duplicate key
	assert(!tree.insert(25));
	assert(tree.getSize() == 1);
	assert(tree.root()->keys[0] == 25);
	assert(tree.root()->keys.getSize() == 1);
	assert(tree.root()->children[0] == nullptr);
	assert(tree.root()->children.getSize() == 2);
	assert(tree.root()->parent == nullptr);

	//insert later keys
	assert(tree.insert(insert_sequence[ix++]));
	assert(tree.getSize() == 2);
	assert(tree.root()->keys[0] == 25);
	assert(tree.root()->keys[1] == 36);
	assert(tree.root()->keys.getSize() == 2);
	assert(tree.root()->children[0] == nullptr);
	assert(tree.root()->children.getSize() == 3);
	assert(tree.root()->parent == nullptr);

	//first split
	assert(tree.insert(insert_sequence[ix++]));
	assert(tree.insert(insert_sequence[ix++]));
	assert(tree.root()->keys[0] == 36);
	assert(tree.root()->keys.getSize() == 1);
	assert(tree.root()->children.getSize() == 2);
	assert(tree.root()->parent == nullptr);
	auto first = tree.root()->children[0];
	auto second = tree.root()->children[1];
	assert(first->keys.getSize() == 2);
	assert(first->children.getSize() == 3);
	assert(first->children[0] == nullptr);
	assert(first->parent == tree.root());
	assert(second->keys.getSize() == 1);
	assert(second->children.getSize() == 2);
	assert(second->children[0] == nullptr);
	assert(second->parent == tree.root());

	//consecutive insert
	for (; ix != 11; ++ix)
		assert(tree.insert(insert_sequence[ix]));
	assert(tree.getSize() == 11);

	auto root = tree.root();
	assert(root->keys[0] == 36);
	assert(root->keys.getSize() == 1);
	assert(root->children.getSize() == 2);
	assert(root->parent == nullptr);
	first = root->children[0];
	assert(first->keys.getSize() == 2);
	assert(first->keys[0] == 19 && first->keys[1] == 25);
	assert(first->children.getSize() == 3);
	assert(first->parent == root);
	second = root->children[1];
	assert(second->keys.getSize() == 1);
	assert(second->keys[0] == 72);
	assert(second->children.getSize() == 2);
	assert(second->parent == root);
	auto firstLeft = first->children[0];
	assert(firstLeft->keys.getSize() == 2);
	assert(firstLeft->keys[0] == 5 && firstLeft->keys[1] == 17);
	assert(firstLeft->children[0] == nullptr && firstLeft->children.getSize() == 3);
	assert(firstLeft->parent == first);
	auto firstMid = first->children[1];
	assert(firstMid->keys.getSize() == 1);
	assert(firstMid->keys[0] == 22);
	assert(firstMid->children.getSize() == 2 && firstMid->children[0] == nullptr);
	assert(firstMid->parent == first);
	auto firstRight = first->children[2];
	assert(firstRight->keys.getSize() == 1);
	assert(firstRight->keys[0] == 32);
	assert(firstRight->children.getSize() == 2 && firstRight->children[0] == nullptr);
	assert(firstRight->parent == first);
	auto secondLeft = second->children[0];
	assert(secondLeft->keys.getSize() == 2);
	assert(secondLeft->keys[0] == 48 && secondLeft->keys[1] == 67);
	assert(secondLeft->children.getSize() == 3 && secondLeft->children[0] == nullptr);
	assert(secondLeft->parent == second);
	auto secondRight = second->children[1];
	assert(secondRight->keys.getSize() == 1);
	assert(secondRight->keys[0] == 99);
	assert(secondRight->children.getSize() == 2 && secondRight->children[0] == nullptr);
	assert(secondLeft->parent == second && secondRight->parent == second);

	//test duplicate insert
	for (ix = 0; ix != 11; ++ix)
		assert(!tree.insert(insert_sequence[ix]));
	assert(tree.getSize() == 11);
}

void test_remove(){
	BTree<int> tree(4);
	int insert_sequence[] = { 25, 36, 17, 48, 32, 5, 67, 72, 99, 19, 22 };
	int ix = 0;
	//insert and remove
	tree.insert(7);
	assert(!tree.remove(14));
	assert(tree.remove(7));
	assert(tree.getSize() == 0);
	assert(tree.root()->keys.getSize() == 0);
	assert(tree.root()->children.getSize() == 1);
	assert(tree.root()->children[0] == nullptr);

	//look around left and right
	for (; ix != 4; ++ix)
		tree.insert(insert_sequence[ix]);
	assert(tree.remove(36));
	assert(tree.getSize() == 3);
	assert(tree.root()->keys[0] == 25);
	assert(tree.root()->keys.getSize() == 1);
	assert(tree.root()->children.getSize() == 2);
	auto first = tree.root()->children[0];
	auto second = tree.root()->children[1];
	assert(first->keys.getSize() == 1 && first->keys[0] == 17);
	assert(first->children.getSize() == 2 && first->children[0] == nullptr);
	assert(second->keys.getSize() == 1 && second->keys[0] == 48);
	assert(second->children.getSize() == 2 && second->children[0] == nullptr);

	//merge left to root
	assert(tree.remove(25));
	assert(tree.getSize() == 2); 
	assert(tree.root()->keys[0] == 17 && tree.root()->keys[1] == 48);
	assert(tree.root()->keys.getSize() == 2);
	assert(tree.root()->children.getSize() == 3);
	assert(tree.root()->children[0] == nullptr);

	//clear tree
	assert(tree.remove(17));
	assert(tree.remove(48));
	assert(tree.getSize() == 0);

	//merge right to root
	for(ix = 0; ix != 4; ++ix)
		tree.insert(insert_sequence[ix]);
	tree.remove(36);
	assert(tree.remove(17));
	assert(tree.getSize() == 2);
	assert(tree.root()->keys[0] == 25 && tree.root()->keys[1] == 48);
	assert(tree.root()->keys.getSize() == 2);
	assert(tree.root()->children.getSize() == 3);
	assert(tree.root()->children[0] == nullptr);

	//clear tree
	assert(tree.remove(25));
	assert(tree.remove(48));
	assert(tree.getSize() == 0);


	//general tests
	for (ix = 0; ix != 11; ++ix)
		tree.insert(insert_sequence[ix]);

	assert(tree.remove(48));
	assert(tree.getSize() == 10);
	assert(tree.remove(36));
	auto root = tree.root();
	assert(root->keys.getSize() == 1 && root->keys[0] == 25);
	assert(root->children.getSize() == 2);
	auto left = root->children[0];
	auto right = root->children[1];
	assert(left->keys.getSize() == 1 && left->keys[0] == 19);
	assert(left->children.getSize() == 2);
	assert(right->keys.getSize() == 1 && right->keys[0] == 67);
	assert(right->children.getSize() == 2);
	auto leftleft = left->children[0];
	auto leftright = left->children[1];
	assert(leftleft->keys.getSize() == 2 && leftleft->keys[0] == 5 && leftleft->keys[1] == 17);
	assert(leftleft->children.getSize() == 3 && leftleft->children[0] == nullptr);
	assert(leftright->keys.getSize() == 1 && leftright->keys[0] == 22);
	assert(leftright->children.getSize() == 2 && leftright->children[0] == nullptr);
	auto rightleft = right->children[0];
	auto rightright = right->children[1];
	assert(rightright->keys.getSize() == 2 && rightright->keys[0] == 72 && rightright->keys[1] == 99);
	assert(rightright->children.getSize() == 3 && rightright->children[0] == nullptr);
	assert(rightleft->keys.getSize() == 1 && rightleft->keys[0] == 32);
	assert(rightleft->children.getSize() == 2 && leftright->children[0] == nullptr);
}
