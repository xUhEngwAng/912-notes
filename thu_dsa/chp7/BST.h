#ifndef BST_H_
#define BST_H_

#include "../chp5/binTree.h"
#include <stdexcept>

using std::runtime_error;

template <typename K, typename V>
class entry {
public:
	K  key;
	V  value;

	//constructor
	entry() = default;
	entry(K k, V v) : key(k), value(v) {}

	//overload operator
	bool operator==(entry<K, V> const e) { return key == e.key; }
	bool operator!=(entry<K, V> const e) { return key != e.key; }
	bool operator>(entry<K, V> const e) { return key > e.key; }
	bool operator<(entry<K, V> const e) { return key < e.key; }
};

#define T entry<K, V>
template <typename K, typename V>
class BST: public BinTree<T>{
protected:
	BinNodePosi(T)  __hot;
	BinNodePosi(T)& searchIn(BinNodePosi(T)& x, K const &key, BinNodePosi(T) &hot);
	BinNodePosi(T)  connect34(BinNodePosi(T) x,  BinNodePosi(T) y,  BinNodePosi(T) z,
							  BinNodePosi(T) T0, BinNodePosi(T) T1, BinNodePosi(T) T2, BinNodePosi(T) T3);	//return new root
	BinNodePosi(T)  rotateAt(BinNodePosi(T) x);																//return new root
	BinNodePosi(T)  removeAt(BinNodePosi(T) &x, BinNodePosi(T) &hot);


public:
	//call by key, always assume that key exists, read-only
	V operator[](K const &key);				

	virtual BinNodePosi(T)& search(K const &key);
	virtual BinNodePosi(T)  insert(K const &key, V const &value);
	virtual bool            remove(K const &key);
};

//protected methods
template <typename K, typename V>
BinNodePosi(T)& BST<K, V>::searchIn(BinNodePosi(T)& x, K const &key, BinNodePosi(T) &hot){
	if (!x || x->data.key == key) return x;
	hot = x;
	return key < x->data.key ? searchIn(x->leftChild, key, hot) : searchIn(x->rightChild, key, hot);
}

template <typename K, typename V>
BinNodePosi(T) BST<K, V>::connect34(BinNodePosi(T) x,  BinNodePosi(T) y,  BinNodePosi(T) z,
									BinNodePosi(T) T0, BinNodePosi(T) T1, BinNodePosi(T) T2, BinNodePosi(T) T3){
	x->leftChild = T0;
	x->rightChild = T1;
	if (T0) T0->parent = x;
	if (T1) T1->parent = x;
	updateHeight(x);

	z->leftChild = T2;
	z->rightChild = T3;
	if (T2) T2->parent = z;
	if (T3) T3->parent = z;
	updateHeight(z);

	x->parent = y;
	z->parent = y;
	y->leftChild = x;
	y->rightChild = z;
	updateHeight(y);
	return y;
}

template <typename K, typename V>
BinNodePosi(T) BST<K, V>::rotateAt(BinNodePosi(T) x){
	BinNodePosi(T) p = x->parent;
	BinNodePosi(T) g = p->parent;
	if(p == g->leftChild){
		if (x == p->leftChild) {
			p->parent = g->parent;
			return connect34(x, p, g, x->leftChild, x->rightChild, p->rightChild, g->rightChild);
		}
		else {
			x->parent = g->parent;
			return connect34(p, x, g, p->leftChild, x->leftChild, x->rightChild, g->rightChild);
		}
	}else{
		if (x == p->leftChild) {
			x->parent = g->parent;
			return connect34(g, x, p, g->leftChild, x->leftChild, x->rightChild, p->rightChild);
		}
		else {
			p->parent = g->parent;
			return connect34(g, p, x, g->leftChild, p->leftChild, x->leftChild, x->rightChild);
		}
	}
}

template <typename K, typename V>
BinNodePosi(T) BST<K, V>::removeAt(BinNodePosi(T) &x, BinNodePosi(T) &hot){
	BinNodePosi(T) succ;
	if      (!x->leftChild)  succ = x = x->rightChild;
	else if (!x->rightChild) succ = x = x->leftChild;
	else{
		succ = x->succ();
		x->data = succ->data;
		if (succ->parent == x) succ->parent->rightChild = succ->rightChild;
		else                   succ->parent->leftChild  = succ->rightChild;
		hot  = succ->parent;
		succ = succ->rightChild;
	}
	if(succ) succ->parent = hot;
	return succ;
}

//public interfaces

template <typename K, typename V>
V BST<K, V>::operator[](K const &key){
	BinNodePosi(T) x = search(key);
	if (!x) throw runtime_error("Fatal Error: key doesn't exist.");
	return x->data.value;
}

template <typename K, typename V>
BinNodePosi(T)& BST<K, V>::search(K const &key){
	return searchIn(__root, key, __hot = nullptr);
}

template <typename K, typename V>
BinNodePosi(T) BST<K, V>::insert(K const &key, V const &value){
	BinNodePosi(T) &x = search(key);
	if (x) return x;

	x = new BinNode<T>(entry<K, V>(key, value), __hot);
	++__size;
	updateHeightAbove(__hot);
	return x;
}

template <typename K, typename V>
bool BST<K, V>::remove(K const &key){
	BinNodePosi(T) &x = search(key);
	if (!x) return false;

	removeAt(x, __hot);
	--__size;
	updateHeightAbove(__hot);
	return true;
}

#undef T

#endif
