#ifndef BINTREE_H_
#define BINTREE_H_

#include "binNode.h"

#define FROMPARENTTO(X) (X->parent?__root: X==X->parent->leftChild? X->parent->leftChild: X->parent->rightChild)
#define STATURE(X)      ((X)?(X)->height: -1)
#define MAX(X, Y)       ((X)>(Y)?(X):(Y))

template <typename T>
class BinTree{
protected:
	BinNodePosi(T) __root;
	int            __size;
	virtual bool updateHeight(BinNodePosi(T) x);
	virtual void updateHeightAbove(BinNodePosi(T) x);
	BinNodePosi(T) higherChild(BinNodePosi(T) x);

public:
	//constructor
	BinTree(): __root(nullptr), __size(0) {};

	//deconstructor
	~BinTree()=default;

	bool           empty() const { return __size == 0; }
	int            size()  const { return __size; }
	BinNodePosi(T) root()  const { return __root; }
	BinNodePosi(T) insertAsRoot(T const &val);
	BinNodePosi(T) insertAsLC(BinNodePosi(T) x, T const &val);				//always assume that this.leftChild   == nullptr
	BinNodePosi(T) insertAsRC(BinNodePosi(T) x, T const &val);				//always assume that this.rightChild  == nullptr

};

//protected methods

template <typename T>
bool BinTree<T>::updateHeight(BinNodePosi(T) x){
	int prevHeight  = x->height;
	int leftHeight  = x->leftChild ?  x->leftChild->height  : -1;
	int rightHeight = x->rightChild ? x->rightChild->height : -1;
	x->height = MAX(leftHeight, rightHeight) + 1;
	
	return prevHeight != x->height;
}

template <typename T>
void BinTree<T>::updateHeightAbove(BinNodePosi(T) x){
	for (; x && updateHeight(x); x = x->parent);
}

template <typename T>
BinNodePosi(T) BinTree<T>::higherChild(BinNodePosi(T) x){
	if (!x->leftChild) return x->rightChild;
	if (!x->rightChild) return x->leftChild;
	return x->leftChild->height < x->rightChild->height ? x->rightChild : x->leftChild;
}

//public interfaces

template <typename T>
BinNodePosi(T) BinTree<T>::insertAsRoot(T const &val){
	__size = 1;
	return __root = new BinNode<T>(val);
}

template <typename T>
BinNodePosi(T) BinTree<T>::insertAsLC(BinNodePosi(T) x, T const &val){
	++__size;
	BinNodePosi(T) res = x->insertAsLC(val);
	updateHeightAbove(x);
	return res;
}

template <typename T>
BinNodePosi(T) BinTree<T>::insertAsRC(BinNodePosi(T) x, T const &val) {
	++__size;
	BinNodePosi(T) res =  x->insertAsRC(val);
	updateHeightAbove(x);
	return res;
}

#endif
