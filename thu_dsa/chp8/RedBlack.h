#ifndef REDBLACK_H_
#define REDBLACK_H_

#include "../chp7/BST.h"

#define ISBLACK(x) (!x || x->color == BLACK)
#define ISRED(x)   (x  && x->color == RED)
#define BLACK_HEIGHT(x) (x? x->height: 0)
#define BLACK_HEIGHT_CHANGED(x) (x && BLACK_HEIGHT(x->leftChild) != BLACK_HEIGHT(x->rightChild))

#define T entry<K, V>
template <typename K, typename V>
class RedBlack: public BST<K, V>{
protected:
	BinNodePosi(T) zig(BinNodePosi(T) p);						//right rotation
	BinNodePosi(T) zag(BinNodePosi(T) p);						//left  rotation
	void solveDoubleRed(BinNodePosi(T) x);
	void solveDoubleBlack(BinNodePosi(T) x);
	bool updateHeight(BinNodePosi(T) x);

public:
	BinNodePosi(T) insert(K const &key, V const &value);
	bool           remove(K const &key);
};

// protected methods

template <typename K, typename V>
BinNodePosi(T) RedBlack<K, V>::zig(BinNodePosi(T) p){
	BinNodePosi(T) x = p->leftChild;
	p->leftChild = x->rightChild;
	if (x->rightChild) x->rightChild->parent = p;

	x->rightChild = p;
	p->parent = x;
	return x;
}

template <typename K, typename V>
BinNodePosi(T) RedBlack<K, V>::zag(BinNodePosi(T) p) {
	BinNodePosi(T) x = p->rightChild;
	p->rightChild = x->leftChild;
	if (x->leftChild) x->leftChild->parent = p;

	x->leftChild = p;
	p->parent = x;
	return x;
}

template <typename K, typename V>
bool RedBlack<K, V>::updateHeight(BinNodePosi(T) x){
	if (!x) return false;
	//else
	int prevHeight = BLACK_HEIGHT(x);
	if (ISBLACK(x->leftChild))
		x->height = BLACK_HEIGHT(x->leftChild) + 1;
	else x->height = BLACK_HEIGHT(x->leftChild);

	return prevHeight != x->height;
}

template <typename K, typename V>
void RedBlack<K, V>::solveDoubleRed(BinNodePosi(T) x){
	BinNodePosi(T) p = x->parent;
	if (!p) { x->color = BLACK; return; }									//x is __root
	if (ISBLACK(p)) return;
	//Double Red Exception
	BinNodePosi(T) g = p->parent;											//assert: g exists, for p is red
	BinNodePosi(T) s = (p == g->leftChild ? g->rightChild : g->leftChild);	//s is the uncle of x

	//case 1: s is black
	if (ISBLACK(s)) {
		BinNodePosi(T) gg      = g->parent;
		BinNodePosi(T) newRoot = rotateAt(x);
		if (gg)
			(g == gg->leftChild ? gg->leftChild : gg->rightChild) = newRoot;
		else __root = newRoot;

		newRoot->color = BLACK;
		newRoot->leftChild->color = RED;
		newRoot->rightChild->color = RED;
		updateHeight(newRoot->leftChild);
		updateHeight(newRoot->rightChild);
		updateHeight(newRoot);
		return;
	}
	//case2: s is red
	else {
		g->color = RED;
		s->color = BLACK;
		p->color = BLACK;
		updateHeight(g);
		solveDoubleRed(g);
		return;
	}
}

template <typename K, typename V>
void RedBlack<K, V>::solveDoubleBlack(BinNodePosi(T) x){
	if (x == __root) return;			//recursion base
	//else
	BinNodePosi(T) p = (x ? x->parent : __hot);
	BinNodePosi(T) s = (x == p->leftChild ? p->rightChild : p->leftChild);

	//case one: s has at least one red child
	if(ISRED(s->leftChild) || ISRED(s->rightChild)){
		BinNodePosi(T) redChild = ISRED(s->leftChild) ? s->leftChild : s->rightChild;
		BinNodePosi(T) g = p->parent;
		BinNodePosi(T) newRoot = rotateAt(redChild);
		if (g)
			(p == g->leftChild ? g->leftChild : g->rightChild) = newRoot;
		else __root = newRoot;
		redChild->color = BLACK;
		updateHeight(newRoot->leftChild);
		updateHeight(newRoot->rightChild);
		updateHeight(newRoot);
		return;
	}
	//case two: p is red
	if(ISRED(p)){
		p->color = BLACK;
		s->color = RED;
		updateHeight(p);
		return;
	}
	//case three: p is black
	if(ISBLACK(p) && ISBLACK(s)){
		p->color = BLACK;
		s->color = RED;
		updateHeight(p);
		solveDoubleBlack(p);
		return;
	}
	//case four: s is red
	if(ISRED(s)){
		p->color = RED;
		s->color = BLACK;
		BinNodePosi(T) g = p->parent;
		if(s == p->leftChild) zig(p);
		else                  zag(p);
		s->parent = g;
		if (g) (p == g->leftChild ? g->leftChild : g->rightChild) = s;
		else                        __root = s;
		solveDoubleBlack(x);
	}

}

// public interfaces
template <typename K, typename V>
BinNodePosi(T) RedBlack<K, V>::insert(K const &key, V const &value){
	BinNodePosi(T) &x = search(key);
	if (x) return x;
	//else
	x = new BinNode<T>(entry<K, V>(key, value), __hot, RED);
	x->height = 1;			//external black height
	++__size;
	solveDoubleRed(x);
	return x;
}

template <typename K, typename V>
bool RedBlack<K, V>::remove(K const &key){
	BinNodePosi(T) &x = search(key);
	if (!x) return false;
	//else
	BinNodePosi(T) succ = removeAt(x, __hot);
	--__size;
	if (ISRED(succ)) {
		succ->color = BLACK;
		return true;
	}
	if (!BLACK_HEIGHT_CHANGED(__hot)) return true;
	solveDoubleBlack(succ);
	return true;
}

#undef T

#endif
