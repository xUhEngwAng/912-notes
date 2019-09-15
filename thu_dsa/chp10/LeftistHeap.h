#ifndef LEFTISTHEAP_H_
#define LEFTISTHEAP_H_

#include "PriorityQueue.h"
#include "../chp5/binTree.h"

#define NPL(X) (X == nullptr? 0: X->npl)

template <typename K, typename V>
class LeftHeap: public PriorityQueue<K, V>, public BinTree<entry<K, V>>{
public:
	//constructor
	LeftHeap() = default;
	LeftHeap(entry<K, V>* src, int n) { for (int ix = 0; ix != n; ++ix) insert(src[ix]); }		//brute force algorithm

	//public interfaces
	int size() { return BinTree<entry<K, V>>::size(); }
	bool empty() { return size() == 0; }
	entry<K, V> getMax() { return __root->data; }
	entry<K, V> delMax();
	void        insert(entry<K, V> e);
	void        insert(K key, V value) { insert(entry<K, V>(key, value)); }
};

#define T entry<K, V>

template<typename K, typename V>
BinNodePosi(T) merge(BinNodePosi(T) a, BinNodePosi(T) b){
	if (a == nullptr) return b;
	if (b == nullptr) return a;
	BinNodePosi(T) temp;
	if (a->data < b->data){//swap a and b
		temp = a;
		a = b;
		b = temp;
	}
	if (a->rightChild == nullptr) a->rightChild = b;
	else a->rightChild = merge(a->rightChild, b);
	a->rightChild->parent = a;

	if (NPL(a->leftChild) < NPL(a->rightChild)) {// swap the right chlid and left child of a
		temp = a->leftChild;
		a->leftChild = a->rightChild;
		a->rightChild = temp;
	}
	a->npl = NPL(a->rightChild) + 1;
	return a;
}

//public interfaces

template <typename K, typename V>
void LeftHeap<K, V>::insert(entry<K, V> e){
	BinNodePosi(T) newNode = new BinNode<T>(e);
	newNode->npl = 1;
	__root = merge(__root, newNode);
	__root->parent = nullptr;
	++__size;
}

template <typename K, typename V>
entry<K, V> LeftHeap<K, V>::delMax(){
	entry<K, V> res = getMax();
	__root = merge(__root->leftChild, __root->rightChild);
	if (__root) __root->parent = nullptr;
	--__size;
	return res;
}

#undef  T

#endif
