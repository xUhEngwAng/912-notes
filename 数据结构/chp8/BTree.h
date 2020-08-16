#ifndef BTREE_H_
#define BTREE_H_

#include "BTNode.h"

template <typename T>
class BTree{
protected:
	BTNodePosi(T) __root;
	BTNodePosi(T) __hot;
	int __order = 3;								//default to 2-3 Btree
	int __size  = 0;

	//protected methods

	void solveOverflow(BTNodePosi(T) x);
	void solveUnderflow(BTNodePosi(T) x);

public:
	//constructor

	BTree() { __root = new BTNode<T>(); }
	BTree(int order) : __order{ order } { __root = new BTNode<T>(); }

	//public interfaces
	int           getSize() { return __size; }
	BTNodePosi(T) root() { return __root; }
	BTNodePosi(T) search(T const &key);
	bool          insert(T const &key);
	bool          remove(T const &key);
};

//protected methods

template <typename T>
void BTree<T>::solveOverflow(BTNodePosi(T) x){
	while (x->keys.getSize() == __order) {
		int mid = __order / 2;
		BTNodePosi(T) newLeaf = new BTNode<T>();
		BTNodePosi(T) p = x->parent;

		//construct split node
		int ix = mid + 1;
		newLeaf->children[0] = x->children[ix];
		if (x->children[ix]) x->children[ix]->parent = newLeaf;
		for(;ix != __order; ++ix){
			newLeaf->keys.push_back(x->keys[ix]);
			newLeaf->children.push_back(x->children[ix + 1]);
			if (x->children[ix + 1]) x->children[ix + 1]->parent = newLeaf;
		}

		if(!p){//construct new __root
			__root = new BTNode<T>(x->keys[mid], x, newLeaf);
			x->parent = __root;
			newLeaf->parent = __root;
			p = __root;
		}
		else {
			int pos = p->keys.search(x->keys[mid]);
			p->keys.insert(pos + 1, x->keys[mid]);
			p->children.insert(pos + 2, newLeaf);
			newLeaf->parent = p;
		}

		x->keys.pop(mid, __order);
		x->children.pop(mid + 1, __order + 1);

		x = p;
	}
}

template <typename T>
void BTree<T>::solveUnderflow(BTNodePosi(T) x){
	while (x != __root && x->children.getSize() == ((__order - 1) >> 1)) {
		BTNodePosi(T) p = x->parent;
		int pos = 0;
		while (p->children[pos] != x) ++pos;
		BTNodePosi(T) leftSibling  = (pos != 0 ? p->children[pos - 1] : nullptr);
		BTNodePosi(T) rightSibling = (pos != p->children.getSize() - 1 ? p->children[pos + 1] : nullptr);

		//look around left and right
		if (leftSibling && leftSibling->children.getSize() > ((__order + 1) >> 1)) {
			x->keys.insert(0, p->keys[pos - 1]);
			x->children.insert(0, leftSibling->children.pop_back());
			if (x->children[0]) x->children[0]->parent = x;

			p->keys[pos - 1] = leftSibling->keys.pop_back();
			return;
		}
		else 
		if (rightSibling && rightSibling->children.getSize() > ((__order + 1) >> 1)) {
				x->keys.push_back(p->keys[pos + 1]);
				x->children.push_back(rightSibling->children.pop(0));
				if (x->children[x->children.getSize() - 1]) x->children[x->children.getSize() - 1]->parent = x;

				p->keys[pos + 1] = rightSibling->keys.pop(0);
				return;
		}
		else {
			if (leftSibling) {//merge left sibling
				leftSibling->keys.push_back(p->keys[pos - 1]);
				int ix = 0;
				for (; ix != x->keys.getSize(); ++ix) {
					leftSibling->keys.push_back(x->keys[ix]);
					leftSibling->children.push_back(x->children[ix]);
					if (x->children[ix]) x->children[ix]->parent = leftSibling;
 				}
				leftSibling->children.push_back(x->children[ix]);
				if (x->children[ix]) x->children[ix]->parent = leftSibling;

				p->keys.pop(pos - 1);
				p->children.pop(pos);
				delete x;
			}
			else {			//merge right sibling
				x->keys.push_back(p->keys[pos]);
				int ix = 0;
				for (; ix != rightSibling->keys.getSize(); ++ix) {
					x->keys.push_back(rightSibling->keys[ix]);
					x->children.push_back(rightSibling->children[ix]);
					if (rightSibling->children[ix]) rightSibling->children[ix]->parent = x;
				}
				x->children.push_back(rightSibling->children[ix]);
				if (rightSibling->children[ix]) rightSibling->children[ix]->parent = x;

				p->keys.pop(pos);
				p->children.pop(pos + 1);
				delete rightSibling;
			}
			if (p == __root && p->keys.getSize() == 0) {
				__root = p->children[0];
				__root->parent = 0;
				p = __root;
			}
			x = p;
		}
	}
}

//public interfaces

template <typename T>
BTNodePosi(T) BTree<T>::search(T const &key){
	BTNodePosi(T) x = __root;
	__hot  = nullptr;
	int pos;
	while(x){
		pos = x->keys.search(key);
		if (pos != -1 && x->keys[pos] == key) break;
		//else
		__hot = x;
		x     = x->children[pos + 1];
	}
	return x;
}

template <typename T>
bool BTree<T>::insert(T const &key){
	BTNodePosi(T) x = search(key);
	if (x) return false;
	//else
	x = __hot;
	int pos = x->keys.search(key);
	x->keys.insert(pos + 1 , key);
	x->children.push_back(nullptr);
	++__size;

	solveOverflow(x);
	return true;
}

template <typename T>
bool BTree<T>::remove(T const &key){
	BTNodePosi(T) x = search(key);
	if (!x) return false;
	//else
	int pos = x->keys.search(key);
	if(x->children[0]){						//if x is not a leaf node, find x's succ
		BTNodePosi(T) succ = x->children[pos + 1];
		while (succ->children[0]) succ = succ->children[0];
		x->keys[pos] = succ->keys[0];
		x = succ;
		pos = 0;
	}
	//now x is a leaf node
	x->keys.pop(pos);
	x->children.pop_back();

	--__size;
	solveUnderflow(x);
	return true;
}

#endif
