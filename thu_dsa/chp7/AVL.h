#ifndef AVL_H_
#define AVL_H_

#include "BST.h"

#define BALANCE_FACTOR(X) (STATURE(X->leftChild) - STATURE(X->rightChild))
#define AVLBALANCED(X)    ((-2 < BALANCE_FACTOR(X)) && (BALANCE_FACTOR(X) < 2))
#define T entry<K, V>

template <typename K, typename V>
class AVL: public BST<K, V>{
public:
	BinNodePosi(T) insert(K const &key, V const &value);
	bool           remove(K const &key);
};

//public interfaces

template <typename K, typename V>
BinNodePosi(T) AVL<K, V>::insert(K const &key, V const &value){
	BinNodePosi(T) &pos = search(key);
	if (pos) return pos;
	//else
	pos = new BinNode<T>(entry<K, V>(key, value), __hot);
	for(BinNodePosi(T) x = __hot; x; x = x->parent){
		if (AVLBALANCED(x)) updateHeight(x);
		else{
			BinNodePosi(T) parent  = x->parent;
			BinNodePosi(T) newRoot = rotateAt(higherChild(higherChild(x)));
			if (parent)
				(x == parent->leftChild ? parent->leftChild : parent->rightChild) = newRoot;
			else __root = newRoot;
			break;
		}
	}
	++__size;
	return pos;
}

template <typename K, typename V>
bool AVL<K, V>::remove(K const &key) {
	BinNodePosi(T) &pos = search(key);
	if (!pos) return false;
	//else
	removeAt(pos, __hot);
	for(BinNodePosi(T) x = __hot; x; x = x->parent){
		if (!AVLBALANCED(x)){
			BinNodePosi(T) parent  = x->parent;
			BinNodePosi(T) newRoot = rotateAt(higherChild(higherChild(x)));
			if (parent)
				(x == parent->leftChild ? parent->leftChild : parent->rightChild) = newRoot;
			else __root = newRoot;
			x = newRoot;
		}
		else updateHeight(x);
	}
	--__size;
	return true;
}

#undef T

#endif
