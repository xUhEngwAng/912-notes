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
	BinNodePosi(T)  connect34(BinNodePosi(T), BinNodePosi(T), BinNodePosi(T),
							  BinNodePosi(T), BinNodePosi(T), BinNodePosi(T), BinNodePosi(T));
	BinNodePosi(T)  rotateAt(BinNodePosi(T) x);


public:
	//call by key, always assume that key exists, read-only
	V operator[](K const &key);				

	virtual BinNodePosi(T)& search(K const &key);
	virtual BinNodePosi(T)  insert(K const &key, V const &value);
	virtual BinNodePosi(T)  remove(K const &key);
};

//protected methods
template <typename K, typename V>
BinNodePosi(T)& BST<K, V>::searchIn(BinNodePosi(T)& x, K const &key, BinNodePosi(T) &hot){
	if (!x || x->data.key == key) return x;
	hot = x;
	return key < x->data.key ? searchIn(x->leftChild, key, hot) : searchIn(x->rightChild, key, hot);
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
BinNodePosi(T) BST<K, V>::remove(K const &key){
	BinNodePosi(T)& x = search(key);
	BinNodePosi(T)  succ;
	if (!x) return nullptr;

	if      (!x->leftChild)  succ = x = x->rightChild;
	else if (!x->rightChild) succ = x = x->leftChild;
	else{
		succ = x->succ();
		x->data = succ->data;

		succ->parent == x ? succ->parent->rightChild : succ->parent->leftChild = succ->rightChild;
		__hot = succ;
		succ = __hot->rightChild;
	}
	if (succ) succ->parent = __hot;
	--__size;
	updateHeightAbove(__hot);
	return succ;
}

#undef T

#endif
